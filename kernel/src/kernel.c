#include <../include/kernel.h>


t_log* kernelLogger;
t_config* kernelConfig;

/////////// LA USAN VARIOS PROCESOS "HILOS" /////////////
static uint32_t nextPid ;

///////////  SEMAFOROS MUTEX ////////////////
static pthread_mutex_t nextPidMutex;
static pthread_mutex_t eliminarLista; // provisorio
////////// SEMAFOROS /////////
static sem_t hayPcbsParaAgregarAlSistema;
static sem_t gradoMultiprog;
//Estados
static t_estado* estadoNew;
static t_estado* estadoReady;
//static t_estado* estadoExec;
//static t_estado* estadoBlocked;
static t_estado* estadoExit;

///////////////////////// FUNCIONES UTILITARIAS //////////////////////////////

static void log_transition(const char* prev, const char* post, int pid) {
    char* transicion = string_from_format("\e[1;93m%s->%s\e[0m", prev, post);
    log_info(kernelLogger, "Transición de %s PCB <ID %d>", transicion, pid);
    free(transicion);
}


uint32_t obtener_siguiente_pid(void) 
{
    pthread_mutex_lock(&nextPidMutex);
    uint32_t newNextPid = nextPid++;
    pthread_mutex_unlock(&nextPidMutex);
    
    return newNextPid;
}

//////////////////////////////////////////////////////////////////////////

int main() {
    kernelLogger = log_create(KERNEL_LOG_UBICACION,KERNEL_PROCESS_NAME,true,LOG_LEVEL_INFO);
    kernelConfig = config_create(KERNEL_CONFIG_UBICACION);
    char *kernelIP = config_get_string_value(kernelConfig, "IP");
    char *kernelPort = config_get_string_value(kernelConfig,"PUERTO");
    nextPid++;
    pthread_mutex_init(&nextPidMutex, NULL);

   /////////////////////////////// CONEXION CON CPU /////////////////////////////
/*
    int kernelSocketCPU = conectar_a_servidor(kernelIP, "8001");
    if (kernelSocketCPU == -1) {
        log_error(kernelLogger, "Error al intentar establecer conexión inicial con módulo CPU");

        log_destroy(kernelLogger);

    return -2;
    }

    /////////////////////////////// CONEXION CON FILE_SYSTEM /////////////////////////////

    int kernelSocketFS = conectar_a_servidor(kernelIP, "8003");
    if (kernelSocketFS == -1) {
        log_error(kernelLogger, "Error al intentar establecer conexión inicial con módulo FILE_SYSTEM");

        log_destroy(kernelLogger);

    return -2;
    }

   /////////////////////////////// CONEXION CON MEMORIA /////////////////////////////

    int kernelSocketMemoria = conectar_a_servidor(kernelIP, "8002");
      if (kernelSocketMemoria == -1) {
        log_error(kernelLogger, "Error al intentar establecer conexión inicial con módulo Memoria");

        log_destroy(kernelLogger);

        return -2;
      }
    */
   ////////////////////////////// CONEXION CON CONSOLA //////////////////////////////
    


   int server_fd = iniciar_servidor(kernelIP, kernelPort);
   log_info(kernelLogger,"Servidor listo para recibir al cliente\n");
   inicializar_estructuras();
   aceptar_conexiones_kernel(server_fd);

  

   log_destroy(kernelLogger);
   config_destroy(kernelConfig);
   return 0;
}

void aceptar_conexiones_kernel(const int socketEscucha) 
{ 
    struct sockaddr cliente = {0};
    socklen_t len = sizeof(cliente);
    
    log_info(kernelLogger, "A la escucha de nuevas conexiones en puerto %d", socketEscucha);
    
    for (; ;) {
        
        const int clienteAceptado = accept(socketEscucha, &cliente, &len);
        
        if (clienteAceptado > -1) {
            
          //  int* socketCliente = malloc(sizeof(*socketCliente));
          //  *socketCliente = clienteAceptado;
              
            crear_hilo_cliente_conexion_entrante(clienteAceptado);
            
           
        } 
        else {

            log_error(kernelLogger, "Error al aceptar conexión: %s", strerror(errno));
        }
    }
}

void encolar_en_new_a_nuevo_proceso(int cliente){

    log_info(kernelLogger, "Nuevo proceso en la cola de new \n");
   
    

    // RECIBO LAS INSTRUCCIONES DE CONSOLA
    t_buffer* bufferIntrucciones = buffer_create();

    uint8_t etiqueta = stream_recv_header(cliente);

    if(HEADER_lista_instrucciones == etiqueta){ //Primero se envia la etiqueta (Header) - despues el paquete para saber como deserealizarlo
        stream_recv_buffer(cliente, bufferIntrucciones);
        // DECLARO LAS ESTRUCTURAS ADMINISTRATIVAS PARA ARMAR EL PROCESO
        t_buffer* instructionsBufferCopy = buffer_create_copy(bufferIntrucciones);
        buffer_destroy(bufferIntrucciones);
        uint32_t newPid = obtener_siguiente_pid();

        t_pcb* newPcb = malloc(sizeof(t_pcb*)); // Me rompe pcb_create()
        
        // LE SETEO LOS VALORES BASICOS
        pcb_set_pid(newPcb, newPid);
        pcb_set_instructions_buffer(newPcb, instructionsBufferCopy);

        log_info(kernelLogger, "Creación de nuevo proceso ID %d mediante <socket %d>", pcb_get_pid(newPcb), cliente);

        //////// LE ENVIO A CONSOLA EL PID ///////

        t_buffer* bufferPID = buffer_create();
        buffer_pack(bufferPID, &newPid, sizeof(newPid));
        stream_send_buffer(cliente, HEADER_pid, bufferPID);
        buffer_destroy(bufferPID);

        /////// LO GUARDO EN LA LISTA DE NEW ////////
        /*
        t_estado* estadoNew = malloc(sizeof(estadoNew));
        estadoNew->nombreEstado = NEW;
        estadoNew->listaProcesos = list_create();
        */
        estado_encolar_pcb_atomic(estadoNew, newPcb);// ACA LO LLEVA A NEW
        log_transition("NULL", "NEW", pcb_get_pid(newPcb));
        sem_post(&hayPcbsParaAgregarAlSistema);  // ESTE SEMAFORO LO QUE HACE ES AGREGAR PCB AL SISTEMA qUE CUMPLAN LO DE ARRIBA
    } 
    else 
    {
        log_error(kernelLogger, "Error de empaquetado no llego bien la instruccion");
        log_destroy(kernelLogger);
         exit(EXIT_FAILURE);
        
    }

    


}


void crear_hilo_cliente_conexion_entrante(int* socket) 
{
    pthread_t threadSuscripcion;
    pthread_create(&threadSuscripcion, NULL, encolar_en_new_a_nuevo_proceso, socket);
    pthread_detach(threadSuscripcion);
}

void* hilo_que_libera_pcbs_en_exit(void* args) 
{
    for (;;) {
        
        //sem_wait(estado_get_sem(estadoExit));
        t_pcb* pcbALiberar = malloc(sizeof(pcbALiberar));
        //pcbALiberar = estado_desencolar_primer_pcb_atomic(estadoExit);
        //mem_adapter_finalizar_proceso(pcbALiberar, kernelConfig, kernelLogger);
        log_info(kernelLogger, "Se finaliza PCB <ID %d>", pcb_get_pid(pcbALiberar));
        //pcb_destroy(pcbALiberar);
        sem_post(&gradoMultiprog);
        free(pcbALiberar);
    }
}

void* planificador_largo_plazo(void* args) 
{
   // pthread_t liberarPcbsEnExitTh;
   // pthread_create(&liberarPcbsEnExitTh, NULL, (void*) hilo_que_libera_pcbs_en_exit, NULL);
   // pthread_detach(liberarPcbsEnExitTh);
    
    for(;;) {

        sem_wait(&hayPcbsParaAgregarAlSistema);
        sem_wait(&gradoMultiprog);
                                 
        // t_pcb* pcbQuePasaAReady = estado_desencolar_primer_pcb_atomic(estadoNew);
        //t_pcb* pcbQuePasaAReady = malloc(sizeof(pcbQuePasaAReady));
        pthread_mutex_lock(&eliminarLista);
        t_pcb* pcbQuePasaAReady = list_remove(estadoNew->listaProcesos, 0);
        
        
        //uint32_t* nuevaTablaPaginasSegmentos = mem_adapter_obtener_tabla_pagina(pcbQuePasaAReady, kernelConfig, kernelDevLogger);

        //pcb_set_array_tabla_paginas(pcbQuePasaAReady, nuevaTablaPaginasSegmentos);

        /*if (nuevaTablaPaginasSegmentos == NULL) {
                
            responder_memoria_insuficiente(pcbQuePasaAReady);   //podria pasarnos con el segmento
        } 
        else {*/

                
            //pcb_set_estado_anterior(pcbQuePasaAReady, pcb_get_estado_actual(pcbQuePasaAReady));
                
                //pcb_set_estado_actual(pcbQuePasaAReady, READY);       ROMPE EN ESTADO ACTUAL 
               
                estado_encolar_pcb_atomic(estadoReady, pcbQuePasaAReady);
            
                //char* stringPidsReady = string_pids_ready(estadoReady);
                log_transition("NEW", "READY", pcb_get_pid(pcbQuePasaAReady));
                pthread_mutex_unlock(&eliminarLista);
                //log_info(kernelLogger,  "Cola Ready <%s>: %s", kernel_config_get_algoritmo_planificacion(kernelConfig), stringPidsReady);
                //free(stringPidsReady);
                free(pcbQuePasaAReady);
               // sem_post(estado_get_sem(estadoReady));
            
        //}
        pcbQuePasaAReady = NULL;
        
    }

}

void inicializar_estructuras(void) {
    
  /*  if (kernel_config_es_algoritmo_fifo(kernelConfig)) {
        
       // elegir_pcb = elegir_pcb_segun_fifo;
       // evaluar_desalojo = evaluar_desalojo_segun_fifo;
        //actualizar_pcb_por_bloqueo = actualizar_pcb_por_bloqueo_segun_fifo;
    } else {
        
        log_error(kernelLogger, "No se pudo inicializar el planificador, no se encontró un algoritmo de planificación válido");
        exit(EXIT_FAILURE);
    }
  */ // PLANI CORTO PLAZO
    nextPid = 1;
    //procesoBloqueadoOTerminado = false;
    pthread_mutex_init(&nextPidMutex, NULL);
    //pthread_mutex_init(&procesoBloqueadoOTerminadoMutex, NULL);
    //pthread_mutex_init(&mutexSocketMemoria, NULL);
    //pthread_mutex_init(&estadoEsperandoMemoria, 1);

    // int valorInicialGradoMultiprog = kernel_config_get_grado_multiprogramacion(kernelConfig);
    int valorInicialGradoMultiprog  = 4;
    sem_init(&hayPcbsParaAgregarAlSistema, 0, 0);
    sem_init(&gradoMultiprog, 0, valorInicialGradoMultiprog);
   // sem_init(&dispatchPermitido, 0, 1); plani de corto plazo
    log_info(kernelLogger, "Se inicializa el grado multiprogramación en %d", valorInicialGradoMultiprog);
    
    
    estadoNew = malloc(sizeof(*estadoNew));
    estadoNew->nombreEstado = NEW;
    estadoNew->listaProcesos = list_create();
    
    estadoReady = malloc(sizeof(*estadoReady));
    estadoReady->nombreEstado = READY;
    estadoReady->listaProcesos = list_create();

    estadoExit = malloc(sizeof(*estadoExit));
    estadoExit->nombreEstado = EXIT;
    estadoExit->listaProcesos = list_create();
    

    //estadoNew = estado_create(NEW);
    //estadoReady = estado_create(READY);
    //estadoExec = estado_create(EXEC);
    //estadoExit = estado_create(EXIT);
    //estadoBlocked = estado_create(BLOCK);
    // TODO HACER ESTADO_CREATE son las inicializaciones de arriba

    pthread_t largoPlazoTh;
    pthread_create(&largoPlazoTh, NULL, (void*)planificador_largo_plazo, NULL);
    pthread_detach(largoPlazoTh);

    /*
    pthread_t cortoPlazoTh;
    pthread_create(&cortoPlazoTh, NULL, (void*) planificador_corto_plazo, NULL);
    pthread_detach(cortoPlazoTh);
    */
  

    log_info(kernelLogger, "Se inicializan las estructuras necesarias para los planificadores");
}