#include <../include/kernel.h>


t_log* kernelLogger;
t_config* kernelConfig;

/////////// LA USAN VARIOS PROCESOS "HILOS" /////////////
static uint32_t nextPid ;

///////////  SEMAFOROS MUTEX ////////////////
static pthread_mutex_t nextPidMutex;

//Estados
//static t_estado* estadoNew;
//static t_estado* estadoReady;
//static t_estado* estadoExec;
//static t_estado* estadoBlocked;
//static t_estado* estadoExit;

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
    log_info(kernelLogger, "Creación de nuevo proceso ID %d mediante <socket %d>", pcb_get_pid(newPcb), cliente);

    //////// LE ENVIO A CONSOLA EL PID ///////

    t_buffer* bufferPID = buffer_create();
    buffer_pack(bufferPID, &newPid, sizeof(newPid));
    stream_send_buffer(cliente, HEADER_pid, bufferPID);
    buffer_destroy(bufferPID);

    /////// LO GUARDO EN LA LISTA DE NEW ////////
    t_estado* estadoNew = malloc(sizeof(estadoNew));
    estadoNew->nombreEstado = NEW;
    estadoNew->listaProcesos = list_create();
    estado_encolar_pcb_atomic(estadoNew, newPcb);// ACA LO LLEVA A NEW
    log_transition("NULL", "NEW", pcb_get_pid(newPcb));
    // sem_post(&hayPcbsParaAgregarAlSistema);  // ESTE SEMAFORO LO QUE HACE ES AGREGAR PCB AL SISTEMA qUE CUMPLAN LO DE ARRIBA
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



