#include <../include/kernel.h>


t_log* kernelLogger;
t_kernel_config* kernelConfig;
t_kernel_recurso* recursoConfig;
static t_estado* elegir_pcb;
t_list* tablaGlobalDeArchivosAbiertos;
t_list* tablaGlobalDeSegmentos;
/////////// LA USAN VARIOS PROCESOS "HILOS" /////////////
static uint32_t nextPid ;
static int cantidad_de_recursos;
char* nombre_recurso;
///////////  SEMAFOROS MUTEX ////////////////
pthread_mutex_t mutexTablaGlobalSegmento = PTHREAD_MUTEX_INITIALIZER;
 pthread_mutex_t mutexCantidadRecursos;
 pthread_mutex_t mutexNombreRecurso;
static pthread_mutex_t nextPidMutex;
static pthread_mutex_t eliminarLista; // provisorio
pthread_mutex_t start_mutex = PTHREAD_MUTEX_INITIALIZER;
////////// SEMAFOROS /////////
static sem_t hayPcbsParaAgregarAlSistema;
static sem_t gradoMultiprog;
static sem_t dispatchPermitido;
//Estados
 t_estado* estadoNew;
 t_estado* estadoReady;
 t_estado* estadoExec;
 t_estado* estadoBlocked;
 t_estado* estadoExit;

///////////////////////// FUNCIONES UTILITARIAS /////////////////////////

void log_transition(const char* prev, const char* post, int pid) {   //Da el color amarillo
    char* transicion = string_from_format("\e[1;93m%s->%s\e[0m", prev, post);
    log_info(kernelLogger, "Transición de %s PCB <ID %d>", transicion, pid);
    free(transicion);
}

void setear_tiempo_ready(t_pcb* this){
                pthread_mutex_lock(&start_mutex);
                struct timespec start;
                set_timespec(&start);
                pthread_mutex_unlock(&start_mutex);
                pcb_set_tiempo_en_ready(this, start);
}

uint32_t obtener_siguiente_pid(void) 
{
    pthread_mutex_lock(&nextPidMutex);
    uint32_t newNextPid = nextPid++;
    pthread_mutex_unlock(&nextPidMutex);
    
    return newNextPid;
}



void liberar_segmentos_del_proceso_tabla_global(t_pcb* pcb){

    t_segmento* aux = segmento_create(-1,-1);
    segmento_set_pid(aux, pcb_get_pid(pcb));
    
    pthread_mutex_lock(&mutexTablaGlobalSegmento);
    tablaGlobalDeSegmentos = list_filter_ok(tablaGlobalDeSegmentos,es_el_segmento_pid,aux);
    pthread_mutex_unlock(&mutexTablaGlobalSegmento);


}



//////////////////////////////////////////////////////////////////////////
/////////////////////////////// FUNCION MAIN ////////////////////////////
int main(int argc, char* argv[]) {
    kernelLogger = log_create(KERNEL_LOG_UBICACION,KERNEL_PROCESS_NAME,true,LOG_LEVEL_INFO);
    t_config* kernelConfigPath = config_create(argv[1]);
    tablaGlobalDeArchivosAbiertos = list_create();
    tablaGlobalDeSegmentos = list_create();
    nextPid++;
    pthread_mutex_init(&nextPidMutex, NULL);
    inicio_kernel();
    kernelConfig = kernel_config_initializer(kernelConfigPath);
    cantidad_de_recursos = size_recurso_list(kernel_config_get_recurso(kernelConfig));
    recursoConfig = iniciar_estructuras_de_recursos(cantidad_de_recursos, kernel_config_get_instancias(kernelConfig), kernel_config_get_recurso(kernelConfig));
    /*
   char* ipMemoria = readline(RED BOLD "ESCRIBIR LA IP DE "RESET GREEN BOLD"MEMORIA "RESET BOLD RED" -> " RESET);
   strcpy(kernelConfig->IP_MEMORIA, ipMemoria);
   free(ipMemoria);

   char* ipCPU = readline(RED BOLD "ESCRIBIR LA IP DE "RESET YELLOW BOLD " CPU  "RESET BOLD RED" -> " RESET);
   strcpy(kernelConfig->IP_CPU, ipCPU);
   free(ipCPU);

   char* ipFs = readline(RED BOLD "ESCRIBIR LA IP DE "RESET CYAN BOLD" FILESYSTEM "RESET BOLD RED"-> " RESET);
   strcpy(kernelConfig->IP_FILESYSTEM, ipFs);
   free(ipFs);

     
    char* ipAddress = getIPAddress();
    printf(RED BOLD  "IP DE KERNEL: %s\n" RESET, ipAddress);
    strcpy( kernelConfig->IP_ESCUCHA , ipAddress);
    free(ipAddress);

    */


   /////////////////////////////// CONEXION CON CPU /////////////////////////////
    conectar_a_servidor_cpu_dispatch(kernelConfig,kernelLogger);
    /////////////////////////////// CONEXION CON FILE_SYSTEM /////////////////////////////
    conectar_con_servidor_file_system(kernelConfig,kernelLogger);
   /////////////////////////////// CONEXION CON MEMORIA /////////////////////////////
    conectar_con_servidor_memoria(kernelConfig,kernelLogger);
   ////////////////////////////// CONEXION CON CONSOLA //////////////////////////////
   int server_fd = iniciar_servidor(kernel_config_get_ip_escucha(kernelConfig), kernel_config_get_puerto_escucha(kernelConfig));
   log_info(kernelLogger,"Servidor listo para recibir a los procesos\n");
   inicializar_estructuras();
   aceptar_conexiones_kernel(server_fd);

   log_destroy(kernelLogger);
   config_destroy(kernelConfig);    

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
        
            crear_hilo_cliente_conexion_entrante(clienteAceptado);
        } 
        else {
            log_error(kernelLogger, "Error al aceptar conexión: %s", strerror(errno));
        }
    }
}

////////////////////////////////////// CAMBIOS DE ESTADOS ////////////////////////////////

void encolar_en_new_a_nuevo_proceso(int cliente){

    log_info(kernelLogger,BOLD UNDERLINE CYAN  "Nuevo proceso en la cola de new \n" RESET);
   
    // RECIBO LAS INSTRUCCIONES DE CONSOLA
    t_buffer* bufferIntrucciones = buffer_create();

    uint8_t etiqueta = stream_recv_header(cliente);

    if(HEADER_lista_instrucciones == etiqueta){ //Primero se envia la etiqueta (Header) - despues el paquete para saber como deserealizarlo
        stream_recv_buffer(cliente, bufferIntrucciones);
        // DECLARO LAS ESTRUCTURAS ADMINISTRATIVAS PARA ARMAR EL PROCESO
        t_buffer* instructionsBufferCopy = buffer_create_copy(bufferIntrucciones);
        buffer_destroy(bufferIntrucciones);

    
        uint32_t newPid = obtener_siguiente_pid();
        t_pcb* newPcb = pcb_create(newPid); 
        // LE SETEO LOS VALORES BASICOS
        pcb_set_socket_consola(newPcb, cliente);
        pcb_set_estimacion_anterior(newPcb, kernel_config_get_estimacion_inicial(kernelConfig));
        pcb_set_rafaga_anterior(newPcb, 0);
        pcb_set_instructions_buffer(newPcb, instructionsBufferCopy);
        

        
        log_info(kernelLogger,BOLD UNDERLINE CYAN "Creación de nuevo proceso "RESET UNDERLINE BOLD MAGENTA"ID %d "RESET BOLD UNDERLINE CYAN " mediante <socket %d>", pcb_get_pid(newPcb), cliente);

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
        t_pcb* pcbALiberar = estado_desencolar_primer_pcb_atomic(estadoExit);
        log_info(kernelLogger, "\e[0;32mSe finaliza PCB <ID %d>", pcb_get_pid(pcbALiberar));
        stream_send_empty_buffer(pcb_get_socket_consola(pcbALiberar),HEADER_proceso_terminado);
        pcb_destroy(pcbALiberar);
        sem_post(&gradoMultiprog);
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
                proceso_pasa_a_ready(pcbQuePasaAReady, "NEW");
                pcbQuePasaAReady = NULL;
         

        
    }

}
///////////////////////////////////// FIN DEL PLANIFICADOR DE LARGO PLAZO ////////////////////////////
//////////////////////////////////// COMIENZO DEL PLANIFICADOR DE CORTO PLAZO ////////////////////////

void* atender_pcb(void* args) 
{
    for (;;) {
        bool procesoFueBloqueado = false;
        bool procesoTuvoOutOfMemory = false;
        sem_wait(estado_get_sem(estadoExec));

        pthread_mutex_lock(estado_get_mutex(estadoExec));
        t_pcb* pcb = list_get(estado_get_list(estadoExec), 0);
        pthread_mutex_unlock(estado_get_mutex(estadoExec));

        uint8_t headerAEnviar = HEADER_pcb_a_ejecutar;


        pthread_mutex_lock(&mutexTablaGlobalSegmento); 
        t_segmento* aux = segmento_create(-1,-1);
        segmento_set_pid(aux, pcb_get_pid(pcb));
        pcb_set_lista_de_segmentos(pcb, list_filter_ok(tablaGlobalDeSegmentos,es_el_segmento_pid,aux));
        segmento_destroy(aux);
        pthread_mutex_unlock(&mutexTablaGlobalSegmento); 


        struct timespec start;
        set_timespec(&start);
        cpu_adapter_enviar_pcb_a_cpu(pcb, headerAEnviar, kernelConfig, kernelLogger);
        uint8_t cpuResponse = stream_recv_header(kernel_config_get_socket_dispatch_cpu(kernelConfig)); 
        
        struct timespec end;
        set_timespec(&end);
        
        pcb_set_rafaga_anterior(pcb, obtener_diferencial_de_tiempo_en_milisegundos(end,start) );

        pcb = cpu_adapter_recibir_pcb_actualizado_de_cpu(pcb, cpuResponse, kernelConfig, kernelLogger); 
        
        pcb = estado_desencolar_primer_pcb_atomic(estadoExec);
        

        switch (cpuResponse) {
            
            case HEADER_proceso_desalojado:

                instruccion_yield(pcb);

                break;
                
            case HEADER_proceso_terminado:
                liberar_segmentos_del_proceso_tabla_global(pcb);
                memoria_adapter_enviar_finalizar_proceso(pcb,kernelConfig,kernelLogger,RESET GREEN BOLD "SUCCES" RESET);
                instruccion_exit(pcb);
                break;
                
            case HEADER_Segmentation_fault:
                liberar_segmentos_del_proceso_tabla_global(pcb);
                memoria_adapter_enviar_finalizar_proceso(pcb,kernelConfig,kernelLogger,RESET RED BOLD "SEG_FAULT" RESET);
                instruccion_exit(pcb);
                break;

            case HEADER_proceso_bloqueado:
                sem_post(&dispatchPermitido);
                instruccion_io(pcb);

                break;

            case HEADER_proceso_pedir_recurso:

                procesoFueBloqueado = instruccion_wait(pcb);


                break;

            case HEADER_proceso_devolver_recurso:

                instruccion_signal(pcb);

                break;

            case HEADER_create_segment:

                procesoTuvoOutOfMemory = instruccion_create_segment(pcb);

                break;
            
            case HEADER_delete_segment:

                instruccion_delete_segment(pcb);
                break;
            case HEADER_f_open:
                
                procesoFueBloqueado = instruccion_f_open(pcb);

                break;
            case HEADER_f_close:
                instruccion_f_close(pcb);
                break;
            case HEADER_f_seek:
                instruccion_f_seek(pcb);
                break;
            case HEADER_f_truncate:
                sem_post(&dispatchPermitido);
                instruccion_f_truncate(pcb);
                break;
            case HEADER_f_read:
                sem_post(&dispatchPermitido);
                instruccion_f_read(pcb);
                break;
            case HEADER_f_write:
                sem_post(&dispatchPermitido);
                instruccion_f_write(pcb);
                break;
            default:
                log_error(kernelLogger, "Error al recibir mensaje de CPU");
                break;
        }


        if
        ( 
           (cpuResponse == HEADER_proceso_pedir_recurso && !procesoFueBloqueado && pcb_get_estado_actual(pcb) != EXIT) 
        || (cpuResponse == HEADER_proceso_devolver_recurso && pcb_get_estado_actual(pcb) == EXEC) 
        || (cpuResponse == HEADER_create_segment) && !procesoTuvoOutOfMemory
        || (cpuResponse == HEADER_delete_segment) 
        || (cpuResponse == HEADER_f_open) && !procesoFueBloqueado
        || (cpuResponse == HEADER_f_close)
        || (cpuResponse == HEADER_f_seek)
         // Agrega otro mas
        )
        {

                estado_encolar_pcb_atomic(estadoExec, pcb);
                sem_post(estado_get_sem(estadoExec));
        }else if(
           (cpuResponse == HEADER_proceso_bloqueado) 
        || (cpuResponse == HEADER_f_truncate)  
        || (cpuResponse == HEADER_f_read) 
        || (cpuResponse == HEADER_f_write)){
         // ESTOS LIBERAN ANTES EL CPU DISPATCHPERMITIDO   
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
        

        sem_wait(&dispatchPermitido);
        log_info(kernelLogger, "Se permite dispatch");

        sem_wait(estado_get_sem(estadoReady));
        if( strcmp(kernel_config_get_algoritmo_planificacion(kernelConfig),"FIFO") == 0 ){
            pcbAejecutar = elegir_pcb_segun_fifo(estadoReady); // DEPENDE DEL PLANIFICADOR ACA ESTA FIFO
        } 
        else {
           pcbAejecutar = elegir_pcb_segun_hhrn(estadoReady);
        }
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
    
   // PLANI CORTO PLAZO
    nextPid = 1;
    pthread_mutex_init(&nextPidMutex, NULL);
    pthread_mutex_init(&mutexNombreRecurso, NULL);
    pthread_mutex_init(&mutexCantidadRecursos, NULL);

    int valorInicialGradoMultiprog = kernel_config_get_grado_multiprogramacion(kernelConfig);
    
    sem_init(&hayPcbsParaAgregarAlSistema, 0, 0);
    sem_init(&gradoMultiprog, 0, valorInicialGradoMultiprog);
    sem_init(&dispatchPermitido, 0, 1); 
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
