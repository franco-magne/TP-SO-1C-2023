#include <../include/memoria.h>


t_log *memoriaLogger;
static t_config *memoriaConfigInicial;
t_memoria_config* memoriaConfig;
Segmento* segCompartido;
tabla_de_segmentos* tabla_segmentos;

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
   tabla_segmentos = estado_create();
   inicializar_memoria();
   
   int serverMemoria = iniciar_servidor(memoria_config_get_ip_escucha(memoriaConfig), memoria_config_get_puerto_escucha(memoriaConfig) );
   log_info(memoriaLogger,"Servidor memoria listo para recibir al modulo\n");
   aceptar_conexiones_memoria(serverMemoria);
   

}  
void* atender_conexiones_cpu(void* socket_ptr) {
    int socketCliente = *(int*)socket_ptr;
    log_info(memoriaLogger, "Se acepta conexión de CPU en socket [%d]", socketCliente);
    atender_peticiones_cpu(socketCliente);
    close(socketCliente); // Cerrar el socket cliente después de atender la conexión
    return NULL;
}

void* atender_conexiones_kernel(void* socket_ptr) {
    int socketCliente = *(int*)socket_ptr;
    log_info(memoriaLogger, "Se acepta conexión de Kernel en socket [%d]", socketCliente);
    atender_peticiones_kernel(socketCliente);
    close(socketCliente); // Cerrar el socket cliente después de atender la conexión
    return NULL;
}

void aceptar_conexiones_memoria(const int socketEscucha) {
    struct sockaddr cliente = {0};
    socklen_t len = sizeof(cliente);
    log_info(memoriaLogger, "A la escucha de nuevas conexiones en puerto %d", socketEscucha);

    while (true) {
        const int clienteAceptado = accept(socketEscucha, &cliente, &len);
        if (clienteAceptado > -1) {
            log_info(memoriaLogger, "Cliente aceptado en el puerto");

            uint8_t handshake = stream_recv_header(clienteAceptado);
            if (handshake == HANDSHAKE_cpu) {
                pthread_t threadAtencion;
                pthread_create(&threadAtencion, NULL, atender_conexiones_cpu, &clienteAceptado);
                pthread_detach(threadAtencion);
                cpuSinAtender = false;
            } else if (handshake == HANDSHAKE_kernel) {
                pthread_t threadAtencion;
                pthread_create(&threadAtencion, NULL, atender_conexiones_kernel, &clienteAceptado);
                pthread_detach(threadAtencion);
                kernelSinAtender = false;
            } else {
                log_error(memoriaLogger, "Error al recibir handshake de cliente en socket [%d]", clienteAceptado);
                close(clienteAceptado); // Cerrar el socket cliente en caso de error
            }
        } else {
            log_error(memoriaLogger, "Error al aceptar conexión: %s", strerror(errno));
        }
    }

    log_info(memoriaLogger, "Finalizando el servidor de memoria");
}