#include <../include/kernel.h>


t_log* kernelLogger;
t_kernel_config* kernelConfig;
t_kernel_recurso* recursoConfig;
static t_estado* elegir_pcb;

//static t_preemption_handler evaluar_desalojo;

/////////// LA USAN VARIOS PROCESOS "HILOS" /////////////
static uint32_t nextPid ;
static int cantidad_de_recursos;
static char* nombre_recurso;
///////////  SEMAFOROS MUTEX ////////////////
static pthread_mutex_t mutexCantidadRecursos;
static pthread_mutex_t mutexNombreRecurso;
static pthread_mutex_t nextPidMutex;
static pthread_mutex_t eliminarLista; // provisorio
////////// SEMAFOROS /////////
static sem_t hayPcbsParaAgregarAlSistema;
static sem_t gradoMultiprog;
static sem_t dispatchPermitido;
//Estados
static t_estado* estadoNew;
static t_estado* estadoReady;
static t_estado* estadoExec;
static t_estado* estadoBlocked;
static t_estado* estadoExit;

///////////////////////// FUNCIONES UTILITARIAS //////////////////////////////

static void set_timespec(struct timespec* timespec) 
{
    int retVal = clock_gettime(CLOCK_REALTIME, timespec);
    
    if (retVal == -1) {
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }
}

static uint32_t obtener_diferencial_de_tiempo_en_milisegundos(struct timespec end, struct timespec start) 
{
    const uint32_t SECS_TO_MILISECS = 1000;
    const uint32_t NANOSECS_TO_MILISECS = 1000000;
    return (end.tv_sec - start.tv_sec) * SECS_TO_MILISECS + (end.tv_nsec - start.tv_nsec) / NANOSECS_TO_MILISECS;
}


static void log_transition(const char* prev, const char* post, int pid) {   //Da el color amarillo
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


static void pid_destroyer(void* pidADestruir)
{
    free(pidADestruir);
}

static void* pcb_to_pid_transformer(void* pcbATransformar)
{
    t_pcb* tempPcbATransformar = (t_pcb*) pcbATransformar;
    uint32_t* tempPid = malloc(sizeof(*tempPid));
    
    *tempPid = tempPcbATransformar->pid;

    return (void*) tempPid; 
}


char* string_pids_ready(t_estado* estadoReady)
{
    t_list* tempPidList;
    char* listaPidsString = string_new();
    uint32_t tempPid;

    pthread_mutex_lock(estado_get_mutex(estadoReady));
    tempPidList = list_map(estadoReady->listaProcesos, pcb_to_pid_transformer);
    pthread_mutex_unlock(estado_get_mutex(estadoReady));

    string_append(&listaPidsString, "[");

    for(int i = 0; i < tempPidList->elements_count; i++) {
        
        tempPid = *(uint32_t*)list_get(tempPidList, i);

        char* stringPid = string_itoa(tempPid);
        string_append(&listaPidsString, stringPid);
        free(stringPid);
        
        if(i != tempPidList->elements_count - 1) {

            string_append(&listaPidsString, ", ");
        }
    }

    string_append(&listaPidsString, "]");

    list_destroy_and_destroy_elements(tempPidList, pid_destroyer);
    return listaPidsString;
}

//////////////////////////////////////// Scheduling Algorithms ////////////////////////////////////////
//FIFO      
static t_pcb* elegir_pcb_segun_fifo(t_estado* estado)
{
    return estado_desencolar_primer_pcb_atomic(estado);
}


//////////////////////////////////////////////////////////////////////////
/////////////////////////////// FUNCION MAIN ////////////////////////////
int main(int argc, char* argv[]) {
    kernelLogger = log_create(KERNEL_LOG_UBICACION,KERNEL_PROCESS_NAME,true,LOG_LEVEL_INFO);
    t_config* kernelConfigPath = config_create(argv[1]);

    nextPid++;
    pthread_mutex_init(&nextPidMutex, NULL);

    //kernelConfig = kernel_config_create(argv[1], kernelLogger);
   kernelConfig = kernel_config_initializer(kernelConfigPath);
   cantidad_de_recursos = size_recurso_list(kernel_config_get_recurso(kernelConfig));
  
   recursoConfig = iniciar_estructuras_de_recursos(cantidad_de_recursos, kernel_config_get_instancias(kernelConfig), kernel_config_get_recurso(kernelConfig));
    
   


   /////////////////////////////// CONEXION CON CPU /////////////////////////////
    conectar_a_servidor_cpu_dispatch(kernelConfig,kernelLogger);
   /* int kernelSocketCPU = conectar_a_servidor("127.0.0.1", "8001");
    kernel_config_set_socket_dispatch_cpu(kernelConfig, kernelSocketCPU);
    if (kernelSocketCPU == -1) {
        log_error(kernelLogger, "Error al intentar establecer conexión inicial con módulo CPU");

        log_destroy(kernelLogger);

    return -2;
    }
   */

    /////////////////////////////// CONEXION CON FILE_SYSTEM /////////////////////////////
/*
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
    


   int server_fd = iniciar_servidor(kernel_config_get_ip_escucha(kernelConfig), kernel_config_get_puerto_escucha(kernelConfig));
   log_info(kernelLogger,"Servidor listo para recibir al cliente\n");
   inicializar_estructuras();
   aceptar_conexiones_kernel(server_fd);

  

   log_destroy(kernelLogger);
   config_destroy(kernelConfig);    //OJO falta Free

   return 0;
}
///////////////////////////////  FIN FUNCION MAIN ////////////////////////////


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

        t_pcb* newPcb = pcb_create(newPid); // Me rompe pcb_create()
        
        // LE SETEO LOS VALORES BASICOS
        pcb_set_instructions_buffer(newPcb, instructionsBufferCopy);

        log_info(kernelLogger, "Creación de nuevo proceso ID %d mediante <socket %d>", pcb_get_pid(newPcb), cliente);

        //////// LE ENVIO A CONSOLA EL PID ///////

        t_buffer* bufferPID = buffer_create();
        buffer_pack(bufferPID, &newPid, sizeof(newPid));
        stream_send_buffer(cliente, HEADER_pid, bufferPID);
        buffer_destroy(bufferPID);

        /////// LO GUARDO EN LA LISTA DE NEW ////////
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
/////////////////////////////////////// COMIENZO DE PLANIFICADOR DE LARGO PLAZO /////////////////////////////////
void* hilo_que_libera_pcbs_en_exit(void* args) 
{
    for (;;) {
        
        sem_wait(estado_get_sem(estadoExit));
        t_pcb* pcbALiberar = malloc(sizeof(pcbALiberar));
        pcbALiberar = estado_desencolar_primer_pcb_atomic(estadoExit);
        //mem_adapter_finalizar_proceso(pcbALiberar, kernelConfig, kernelLogger);
        log_info(kernelLogger, "\e[0;32mSe finaliza PCB <ID %d>", pcb_get_pid(pcbALiberar));
        //pcb_destroy(pcbALiberar);
        sem_post(&gradoMultiprog);
        free(pcbALiberar);
    }
}

void* planificador_largo_plazo(void* args) 
{
   pthread_t liberarPcbsEnExitTh;
   pthread_create(&liberarPcbsEnExitTh, NULL, (void*) hilo_que_libera_pcbs_en_exit, NULL);
   pthread_detach(liberarPcbsEnExitTh);
    
    for(;;) {

        sem_wait(&hayPcbsParaAgregarAlSistema);
        sem_wait(&gradoMultiprog);
                                 
        t_pcb* pcbQuePasaAReady = estado_desencolar_primer_pcb_atomic(estadoNew);
        
        
        //uint32_t* nuevaTablaPaginasSegmentos = mem_adapter_obtener_tabla_pagina(pcbQuePasaAReady, kernelConfig, kernelDevLogger);

        //pcb_set_array_tabla_paginas(pcbQuePasaAReady, nuevaTablaPaginasSegmentos);

        /*if (nuevaTablaPaginasSegmentos == NULL) {
                
            responder_memoria_insuficiente(pcbQuePasaAReady);   //podria pasarnos con el segmento
        } 
        else {*/

                
                pcb_set_estado_anterior(pcbQuePasaAReady, pcb_get_estado_actual(pcbQuePasaAReady));
                
                pcb_set_estado_actual(pcbQuePasaAReady, READY);     
               
                estado_encolar_pcb_atomic(estadoReady, pcbQuePasaAReady);
            
                char* stringPidsReady = string_pids_ready(estadoReady);
                log_transition("NEW", "READY", pcb_get_pid(pcbQuePasaAReady));

                log_info(kernelLogger,  "Cola Ready <%s>: %s", kernel_config_get_algoritmo_planificacion(kernelConfig), stringPidsReady);
                free(stringPidsReady);
                sem_post(estado_get_sem(estadoReady));
            
        //}
        pcbQuePasaAReady = NULL;
        
    }

}

bool recurso_disponible(int posicion_recurso){
    return ( *(recursoConfig[posicion_recurso].instancias_recurso) > 0);
}

void asignar_recurso(int posicion_recurso){
     int aux = 1;
    pthread_mutex_lock(&mutexCantidadRecursos);
    *(recursoConfig[posicion_recurso].instancias_recurso) -=aux;
    log_info(kernelLogger, "valor recurso actual : <%i>",*(recursoConfig[posicion_recurso].instancias_recurso) );
    pthread_mutex_unlock(&mutexCantidadRecursos);

}

void devolver_recurso(int posicion_recurso){

    pthread_mutex_lock(&mutexCantidadRecursos);
    *(recursoConfig[posicion_recurso].instancias_recurso) += 1;
    log_info(kernelLogger, "valor recurso actual : <%i>",*(recursoConfig[posicion_recurso].instancias_recurso) );
    pthread_mutex_unlock(&mutexCantidadRecursos);

}

bool pcb_esta_bloqueado_por_recurso(void* pcb){
return (strcmp(pcb_get_recurso_utilizado(pcb), nombre_recurso) == 0);
}

t_pcb* primer_elemento_bloqueado_por_recurso(t_list* listaBloqueado, char* nombreRecurso){
    
    t_pcb* pcb;
    t_list* listAux = list_create();
    listAux = listaBloqueado;
    
    pthread_mutex_lock(&mutexNombreRecurso);
    nombre_recurso = nombreRecurso;
    pthread_mutex_unlock(&mutexNombreRecurso);
    listAux = list_filter(listAux, pcb_esta_bloqueado_por_recurso);
    int cantidadPcbsEnLista = list_size(listAux);
    if(cantidadPcbsEnLista == 1){
         pcb = estado_desencolar_primer_pcb(estadoBlocked);
         return pcb;
    } else if(cantidadPcbsEnLista > 1){
        pcb = list_get(listAux, 0);
        pcb = estado_remover_pcb_de_cola_atomic(estadoBlocked,pcb);
        return pcb;
    }

    
    return NULL;
            
}

///////////////////////////////////// FIN DEL PLANIFICADOR DE LARGO PLAZO ////////////////////////////
//////////////////////////////////// COMIENZO DEL PLANIFICADOR DE CORTO PLAZO ////////////////////////
static bool pedir_recursos_wait(t_pcb* pcb) {
    char* recursoUtilizado = pcb_get_recurso_utilizado(pcb);
    
    if (contains(kernel_config_get_recurso(kernelConfig), recursoUtilizado)) {
        int posicion_recurso = position_in_list(kernel_config_get_recurso(kernelConfig), recursoUtilizado);

        if (recurso_disponible(posicion_recurso)) {
            asignar_recurso(posicion_recurso);
            log_info(kernelLogger, "RECURSO ASIGNADO AL PROCESO <%i>. RECURSO: <%s>", pcb_get_pid(pcb), recursoConfig[posicion_recurso].recurso);
        } else {
            log_transition("EXEC", "BLOCK", pcb_get_pid(pcb));
            log_info(kernelLogger, "PID: <%i> - Bloqueado por: <%s>", pcb_get_pid(pcb), recursoConfig[posicion_recurso].recurso);

            pcb_set_estado_anterior(pcb, pcb_get_estado_actual(pcb));
            pcb_set_estado_actual(pcb, BLOCK);
            estado_encolar_pcb_atomic(estadoBlocked, pcb);

            return true;
        }


    } else {
        log_error(kernelLogger, "RECURSO NO EXISTE POR EL PROCESO QUE PIDE ");    
        pcb_set_estado_actual(pcb, EXIT);
        estado_encolar_pcb_atomic(estadoExit, pcb);
        log_transition("EXEC", "EXIT", pcb_get_pid(pcb));
        //stream_send_empty_buffer(pcb_get_socket(pcb), HEADER_proceso_terminado);
        sem_post(estado_get_sem(estadoExit));
        return false;

    }

}


static void devolver_recursos_signal(t_pcb* pcb){
    
        if(contains(kernel_config_get_recurso(kernelConfig) , pcb_get_recurso_utilizado(pcb)) ){
            
            int posicion_recurso = position_in_list(kernel_config_get_recurso(kernelConfig) , pcb_get_recurso_utilizado(pcb));
            devolver_recurso(posicion_recurso);
                log_info(kernelLogger, "RECURSO DEVUELTO POR EL PROCESO <%i> . RECURSO: <%s> ",pcb_get_pid(pcb), recursoConfig[posicion_recurso].recurso  );    
            t_pcb* pcbPasaReady = primer_elemento_bloqueado_por_recurso(estado_get_list(estadoBlocked), pcb_get_recurso_utilizado(pcb));
            if(pcbPasaReady != NULL){
                pcb_set_estado_anterior(pcbPasaReady, pcb_get_estado_actual(pcbPasaReady));
                pcb_set_estado_actual(pcbPasaReady, READY);
                estado_encolar_pcb_atomic(estadoReady, pcbPasaReady);
                char* stringPidsReady = string_pids_ready(estadoReady);
                log_transition("BLOCK", "READY", pcb_get_pid(pcbPasaReady));
                log_info(kernelLogger,  "Cola Ready <%s>: %s", kernel_config_get_algoritmo_planificacion(kernelConfig), stringPidsReady);
                free(stringPidsReady);
                sem_post(estado_get_sem(estadoReady));
                
            }
        } else {
            log_error(kernelLogger, "RECURSO NO EXISTE POR EL PROCESO QUE LO DEVUELVE ");    
            pcb_set_estado_actual(pcb, EXIT);
            estado_encolar_pcb_atomic(estadoExit, pcb);
            log_transition("EXEC", "EXIT", pcb_get_pid(pcb));
            //stream_send_empty_buffer(pcb_get_socket(pcb), HEADER_proceso_terminado);
            sem_post(estado_get_sem(estadoExit));
        }

}

static void atender_bloqueo_IO(t_pcb* pcb) 
{
    
    log_transition("EXEC", "BLOCK", pcb_get_pid(pcb));
    log_info(kernelLogger, "PCB <ID %d> - Bloqueado por: <%i> segundos", pcb_get_pid(pcb), pcb_get_tiempoIO(pcb));

    log_info(kernelLogger, "PCB <ID %d> ingresa a la cola de espera de I/O de %i", pcb_get_pid(pcb), pcb_get_tiempoIO(pcb));
    
    pcb_set_estado_anterior(pcb, pcb_get_estado_actual(pcb));
    pcb_set_estado_actual(pcb, BLOCK);
    estado_encolar_pcb_atomic(estadoBlocked, pcb);
    
    sleep(pcb_get_tiempoIO(pcb));
    int cantidadPcbsEnLista = list_size(estado_get_list(estadoBlocked));
    
    if (cantidadPcbsEnLista == 1) {
        pcb = estado_desencolar_primer_pcb(estadoBlocked);
    } 
    else {
    pcb = estado_remover_pcb_de_cola_atomic(estadoBlocked,pcb);
    }
    pcb_set_estado_anterior(pcb, pcb_get_estado_actual(pcb));
    pcb_set_estado_actual(pcb, READY);
    estado_encolar_pcb_atomic(estadoReady, pcb);
    char* stringPidsReady = string_pids_ready(estadoReady);
    log_transition("BLOCK", "READY", pcb_get_pid(pcb));
    log_info(kernelLogger,  "Cola Ready <%s>: %s", kernel_config_get_algoritmo_planificacion(kernelConfig), stringPidsReady);
    free(stringPidsReady);
    sem_post(estado_get_sem(estadoReady));
    
}

void* atender_pcb(void* args) 
{
    for (;;) {
        bool procesoFueBloqueado = false;
        sem_wait(estado_get_sem(estadoExec));

        pthread_mutex_lock(estado_get_mutex(estadoExec));
        t_pcb* pcb = list_get(estado_get_list(estadoExec), 0);
        pthread_mutex_unlock(estado_get_mutex(estadoExec));

        uint8_t headerAEnviar = HEADER_pcb_a_ejecutar;
        
        struct timespec start;
        set_timespec(&start);
        //pcb_set_proceso_bloqueado_o_terminado_atomic(pcb, false);

        cpu_adapter_enviar_pcb_a_cpu(pcb, headerAEnviar, kernelConfig, kernelLogger);
        uint8_t cpuResponse = stream_recv_header(kernel_config_get_socket_dispatch_cpu(kernelConfig)); 
        struct timespec end;
        set_timespec(&end);

        pcb = cpu_adapter_recibir_pcb_actualizado_de_cpu(pcb, cpuResponse, kernelConfig, kernelLogger); 
        
        pcb = estado_desencolar_primer_pcb_atomic(estadoExec);

        uint32_t realEjecutado = 0;
        realEjecutado = obtener_diferencial_de_tiempo_en_milisegundos(end, start);
      
        log_debug(kernelLogger, "PCB <ID %d> estuvo en ejecución por %d miliseconds", pcb_get_pid(pcb), realEjecutado);

        switch (cpuResponse) {
            
            case HEADER_proceso_desalojado:
            
                    pcb_set_estado_anterior(pcb, pcb_get_estado_actual(pcb));
                    pcb_set_estado_actual(pcb, READY);
                    estado_encolar_pcb_atomic(estadoReady, pcb);
                    char* stringPidsReady = string_pids_ready(estadoReady);
                    log_transition("EXEC", "READY", pcb_get_pid(pcb));
                    log_info(kernelLogger,  "Cola Ready <%s>: %s", kernel_config_get_algoritmo_planificacion(kernelConfig), stringPidsReady);
                    free(stringPidsReady);
                    sem_post(estado_get_sem(estadoReady));
                

                break;
                
            case HEADER_proceso_terminado:
                
                pcb_set_estado_actual(pcb, EXIT);
                estado_encolar_pcb_atomic(estadoExit, pcb);
                log_transition("EXEC", "EXIT", pcb_get_pid(pcb));
                //stream_send_empty_buffer(pcb_get_socket(pcb), HEADER_proceso_terminado);
                sem_post(estado_get_sem(estadoExit));
                //pcb_set_proceso_bloqueado_o_terminado_atomic(pcb, true);
                //terminar_proceso(pcb);
                break;

            case HEADER_proceso_bloqueado:
                atender_bloqueo_IO(pcb);

                break;
            case HEADER_proceso_pedir_recurso:
                procesoFueBloqueado = pedir_recursos_wait(pcb);

                break;
            case HEADER_proceso_devolver_recurso:
                devolver_recursos_signal(pcb);
                break;

            default:

                log_error(kernelLogger, "Error al recibir mensaje de CPU");
                break;
        }
        if( (cpuResponse == HEADER_proceso_pedir_recurso && !procesoFueBloqueado && pcb_get_estado_actual(pcb) != EXIT) || (cpuResponse == HEADER_proceso_devolver_recurso && pcb_get_estado_actual(pcb) == EXEC) ){
                estado_encolar_pcb_atomic(estadoExec, pcb);
                sem_post(estado_get_sem(estadoExec));
        } 
        else{
                sem_post(&dispatchPermitido);
        }
    }

}

void* planificador_corto_plazo(void* args)
{
    t_pcb* pcbAejecutar = NULL;

    pthread_t atenderPCBThread;
    pthread_create(&atenderPCBThread, NULL, (void*) atender_pcb, NULL);
    pthread_detach(atenderPCBThread);

    for (;;) {
        
        //sem_wait(&evaluarDesalojo);

      // evaluar_desalojo();

        sem_wait(&dispatchPermitido);
        log_info(kernelLogger, "Se permite dispatch");

        sem_wait(estado_get_sem(estadoReady));
        pcbAejecutar = elegir_pcb_segun_fifo(estadoReady); // DEPENDE DEL PLANIFICADOR ACA ESTA FIFO
        log_info(kernelLogger, "Se toma una instancia de READY");
        log_transition("READY", "EXEC", pcb_get_pid(pcbAejecutar));
        
        pcb_set_estado_anterior(pcbAejecutar, pcb_get_estado_actual(pcbAejecutar));
        pcb_set_estado_actual(pcbAejecutar, EXEC);
        estado_encolar_pcb_atomic(estadoExec, pcbAejecutar);
        sem_post(estado_get_sem(estadoExec));

        pcbAejecutar = NULL;
    }
}

/////////////////////////// FIN DEL PLANIFICADOR DE CORTO PLAZO //////////////////////
void inicializar_estructuras(void) {
    
    if (kernel_config_es_algoritmo_fifo(kernelConfig)) {
        
    //elegir_pcb = elegir_pcb_segun_fifo;
    //evaluar_desalojo = evaluar_desalojo_segun_fifo;
    //actualizar_pcb_por_bloqueo = actualizar_pcb_por_bloqueo_segun_fifo;
    } else {
        
        log_error(kernelLogger, "No se pudo inicializar el planificador, no se encontró un algoritmo de planificación válido");
        exit(EXIT_FAILURE);
    }
  
   // PLANI CORTO PLAZO
    nextPid = 1;
    //procesoBloqueadoOTerminado = false;
    pthread_mutex_init(&nextPidMutex, NULL);
    //pthread_mutex_init(&procesoBloqueadoOTerminadoMutex, NULL);
    //pthread_mutex_init(&mutexSocketMemoria, NULL);
    //pthread_mutex_init(&estadoEsperandoMemoria, 1);
    pthread_mutex_init(&mutexNombreRecurso, NULL);
    pthread_mutex_init(&mutexCantidadRecursos, NULL);

    int valorInicialGradoMultiprog = kernel_config_get_grado_multiprogramacion(kernelConfig);
    
    sem_init(&hayPcbsParaAgregarAlSistema, 0, 0);
    sem_init(&gradoMultiprog, 0, valorInicialGradoMultiprog);
    sem_init(&dispatchPermitido, 0, 1); // plani de corto plazo
    log_info(kernelLogger, "Se inicializa el grado multiprogramación en %d", valorInicialGradoMultiprog);
    
 
    estadoNew = estado_create(NEW);
    estadoReady = estado_create(READY);
    estadoExec = estado_create(EXEC);
    estadoExit = estado_create(EXIT);
    estadoBlocked = estado_create(BLOCK);
    
    pthread_t largoPlazoTh;
    pthread_create(&largoPlazoTh, NULL, (void*)planificador_largo_plazo, NULL);
    pthread_detach(largoPlazoTh);

    
    pthread_t cortoPlazoTh;
    pthread_create(&cortoPlazoTh, NULL, (void*) planificador_corto_plazo, NULL);
    pthread_detach(cortoPlazoTh);
    
  

    log_info(kernelLogger, "Se inicializan las estructuras necesarias para los planificadores");
}