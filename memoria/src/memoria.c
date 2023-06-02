#include <../include/memoria.h>


extern t_log *memoriaLogger;
extern t_config *memoriaConfig;

static bool cpuSinAtender;
static bool kernelSinAtender;

void aceptar_conexiones_memoria(const int );

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

void recibir_conexion(int socketEscucha, int* socketCliente, pthread_t* threadSuscripcion) {
    struct sockaddr cliente = {0};
    socklen_t len = sizeof(cliente);
    *socketCliente = accept(socketEscucha, &cliente, &len);
    if (*socketCliente == -1) {
        log_error(memoriaLogger, "Error al aceptar conexion de cliente: %s", strerror(errno));
        exit(-1);
    }
    uint8_t handshake = stream_recv_header(*socketCliente);
    stream_recv_empty_buffer(*socketCliente);

    if (handshake == HANDSHAKE_cpu && cpuSinAtender) {  //solic tabla de segmentos
        log_info(memoriaLogger, "\e[1;92mSe acepta conexión de CPU en socket [%d]\e[0m", *socketCliente);
        t_buffer* buffer = buffer_create();
        uint32_t tamanioSegmento = memoria_config_get_tamanio_segmento(memoriaConfig);
        buffer_pack(buffer, &tamanioSegmento, sizeof(tamanioSegmento));
        stream_send_buffer(*socketCliente, HANDSHAKE_ok_continue, buffer);
        buffer_destroy(buffer);
        atender_peticiones_cpu();
        cpuSinAtender = false;
    } 
    else if (handshake == HANDSHAKE_kernel && kernelSinAtender) {
        log_info(memoriaLogger, "\e[1;92mSe acepta conexión de Kernel en socket [%d]\e[0m", *socketCliente);
        stream_send_empty_buffer(*socketCliente, HANDSHAKE_ok_continue);
        atender_peticiones_kernel(socketCliente); //es void* pero nomas la llamo
        kernelSinAtender = false;
    } 
    else {
        log_error(memoriaLogger, "Error al recibir handshake de cliente: %s", strerror(errno));
        exit(-1);
    }
    return;
}