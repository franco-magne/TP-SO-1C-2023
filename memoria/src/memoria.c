#include <../include/memoria.h>


t_log *memoriaLogger;
static t_config *memoriaConfigInicial;
t_memoria_config* memoriaConfig;
Segmento* segCompartido;
t_estado* tabla_segmentos; //antigua tabla_de_segmentos
t_list* listaDeProcesos;

static bool cpuSinAtender;
static bool kernelSinAtender;
static bool fileSystemSinAtender;
static pthread_t threadAntencionCpu;
static pthread_t threadAntencionKernel;

void aceptar_conexiones_memoria(const int );

int main() {

   memoriaLogger = log_create(MEMORIA_LOG_UBICACION,MEMORIA_PROCESS_NAME,true,LOG_LEVEL_INFO);
   memoriaConfigInicial = config_create(MEMORIA_CONFIG_UBICACION);
   memoriaConfig = memoria_config_initializer(memoriaConfigInicial);
   //tabla_segmentos = estado_create();

   int serverMemoria = iniciar_servidor(memoria_config_get_ip_escucha(memoriaConfig), memoria_config_get_puerto_escucha(memoriaConfig) );
   log_info(memoriaLogger,"Servidor memoria listo para recibir al modulo\n");
   inicializar_memoria();
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
            log_info(memoriaLogger, "Cliente aceptado en el puerto");
            recibir_conexion(clienteAceptado);
        } 
        else {

            log_error(memoriaLogger, "Error al aceptar conexión: %s", strerror(errno));
        }
    }
}

void recibir_conexion(int socketCliente) {
    
    uint8_t handshake = stream_recv_header(socketCliente);
    //stream_recv_empty_buffer(*socketCliente);
    log_info("Handshake recibido %d", handshake);
    
    //deberia restar tamActualMemoria con los mutex

    if (handshake == HANDSHAKE_cpu) {  //solic tabla de segmentos   
        log_info(memoriaLogger, "\e[1;92mSe acepta conexión de CPU en socket [%d]\e[0m", socketCliente);
    
        /*t_buffer* buffer = buffer_create();
        uint32_t tamanioSegmento = memoria_config_get_tamanio_segmento_0(memoriaConfig);
        buffer_pack(buffer, &tamanioSegmento, sizeof(tamanioSegmento));
        stream_send_buffer(socketCliente, HANDSHAKE_ok_continue, buffer);
        buffer_destroy(buffer);
        pthread_create(&threadAntencionCpu, NULL, atender_peticiones_cpu(), socketCliente);
        pthread_detach(threadAntencionCpu);
        cpuSinAtender = false;
        */
    } 

    else if (handshake == HANDSHAKE_kernel) {
        log_info(memoriaLogger, "\e[1;92mSe acepta conexión de Kernel en socket [%d]\e[0m", socketCliente);
        //inicializar_estructuras();
        //estado_encolar_segmento_atomic(tabla_segmentos, segCompartido);
        stream_send_empty_buffer(socketCliente, HANDSHAKE_ok_continue);
        pthread_create(&threadAntencionCpu, NULL, atender_peticiones_kernel, socketCliente);
        pthread_detach(threadAntencionCpu);
        kernelSinAtender = false;
    } 
    /*else if (handshake == HANDSHAKE_fileSystem && fileSystemSinAtender) {
        log_info(memoriaLogger, "\e[1;92mSe acepta conexión de fileSystem en socket [%d]\e[0m", *socketCliente);
        stream_send_empty_buffer(*socketCliente, HANDSHAKE_ok_continue);
        //pthread_create(&threadAntencionCpu, NULL, atender_peticiones_fileSystem(), socketCliente);
        //pthread_detach(threadAntencionCpu);
        kernelSinAtender = false;
    }*/
    else {
        log_error(memoriaLogger, "Error al recibir handshake de cliente: %s", strerror(errno));
        exit(-1);
    }
    return;
}