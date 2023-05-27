#include <../include/memoria.h>


t_log *memoriaLogger;
t_config *memoriaConfig;

t_MemoriaData *memoriaData;
static bool cpuSinAtender;
static bool kernelSinAtender;

int main() {

   memoriaLogger = log_create(MEMORIA_LOG_UBICACION,MEMORIA_PROCESS_NAME,true,LOG_LEVEL_INFO);
   memoriaConfig = config_create(MEMORIA_CONFIG_UBICACION);
   char *memoriaIP = config_get_string_value(memoriaConfig, "IP");
   char *memoriaPort = config_get_string_value(memoriaConfig,"PUERTO_MEMORIA");
   


   int serverMemoria = iniciar_servidor(memoriaIP, memoriaPort);
   log_info(memoriaLogger,"Servidor memoria listo para recibir al modulo\n");
   aceptar_conexiones_memoria(serverMemoria);
   

   

}  

void aceptar_conexiones_memoria(const int socketEscucha) 
{ 
    struct sockaddr cliente = {0};
    socklen_t len = sizeof(cliente);
    
    log_info(memoriaLogger, "A la escucha de nuevas conexiones en puerto %d", socketEscucha);
    
    for (int i =0 ;i<3 ;i++) { // PONEMOS 2 PORQUE PARA TESTEAR QUE SE CONECTE PRIMERO CPU Y DESPUES KERNEL
        
        const int clienteAceptado = accept(socketEscucha, &cliente, &len);
        
        if (clienteAceptado > -1) {
            
            log_info(memoriaLogger, "Se conecto un modulo");        
         } 
        else {

            log_error(memoriaLogger, "Error al aceptar conexión: %s", strerror(errno));
        }
    }
}

void* recibir_conexion(int socketEscucha, int* socketCliente, pthread_t* threadSuscripcion) {
    struct sockaddr cliente = {0};
    socklen_t len = sizeof(cliente);
    *socketCliente = accept(socketEscucha, &cliente, &len);
    if (*socketCliente == -1) {
        log_error(memoriaData->memoriaLogger, "Error al aceptar conexion de cliente: %s", strerror(errno));
        exit(-1);
    }
    uint8_t handshake = stream_recv_header(*socketCliente);
    stream_recv_empty_buffer(*socketCliente);
    //void* (*funcion_suscripcion)(void*) = NULL;
    if (handshake == HANDSHAKE_cpu && cpuSinAtender) {
        log_info(memoriaData->memoriaLogger, "\e[1;92mSe acepta conexión de CPU en socket [%d]\e[0m", *socketCliente);
        t_buffer* buffer = buffer_create();
        uint32_t tamanioPagina = 0;//memoriaData->tamanioPagina;
        //uint32_t entradasPorTabla = memoriaData->entradasPorTabla;
        buffer_pack(buffer, &tamanioPagina, sizeof(tamanioPagina));
        //buffer_pack(buffer, &entradasPorTabla, sizeof(entradasPorTabla));
        stream_send_buffer(*socketCliente, HANDSHAKE_ok_continue, buffer);
        buffer_destroy(buffer);
        //funcion_suscripcion = escuchar_peticiones_cpu;
        cpuSinAtender = false;
    } else if (handshake == HANDSHAKE_kernel && kernelSinAtender) {
        log_info(memoriaData->memoriaLogger, "\e[1;92mSe acepta conexión de Kernel en socket [%d]\e[0m", *socketCliente);
        stream_send_empty_buffer(*socketCliente, HANDSHAKE_ok_continue);
        atender_peticiones_kernel(); //es void* pero nomas la llamo
        kernelSinAtender = false;
    } else {
        log_error(memoriaData->memoriaLogger, "Error al recibir handshake de cliente: %s", strerror(errno));
        exit(-1);
    }
    //return funcion_suscripcion;
}