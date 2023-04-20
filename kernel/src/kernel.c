#include <../include/kernel.h>


t_log* kernelLogger;
t_config* kernelConfig;

int main() {
    kernelLogger = log_create(KERNEL_LOG_UBICACION,KERNEL_PROCESS_NAME,true,LOG_LEVEL_INFO);
    kernelConfig = config_create(KERNEL_CONFIG_UBICACION);
    char *kernelIP = config_get_string_value(kernelConfig, "IP");
    char *kernelPort = config_get_string_value(kernelConfig,"PUERTO");

   /////////////////////////////// CONEXION CON CPU /////////////////////////////

    int kernelSocketCPU = conectar_a_servidor(kernelIP, "8001");
    if (kernelSocketCPU == -1) {
        log_error(kernelLogger, "Error al intentar establecer conexión inicial con módulo Memoria");

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
            
            int* socketCliente = malloc(sizeof(*socketCliente));
            *socketCliente = clienteAceptado;
            crear_hilo_cliente_conexion_entrante(socketCliente);
        } 
        else {

            log_error(kernelLogger, "Error al aceptar conexión: %s", strerror(errno));
        }
    }
}

void encolar_en_new_a_nuevo_cliente(){

   log_info(kernelLogger, "Nuevo cliente en la cola de new \n");

}


void crear_hilo_cliente_conexion_entrante(int* socket) 
{
    pthread_t threadSuscripcion;
    pthread_create(&threadSuscripcion, NULL, encolar_en_new_a_nuevo_cliente, NULL);
    pthread_detach(threadSuscripcion);
}



