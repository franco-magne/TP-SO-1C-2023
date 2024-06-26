#include <../include/memoria.h>


t_log *memoriaLogger;
static t_config *memoriaConfigInicial;
t_memoria_config* memoriaConfig;
Segmento* segCompartido;
t_list* listaDeSegmentos;
void* memoriaPrincipal;
uint32_t tamActualMemoria;

static bool cpuSinAtender = false;
static bool kernelSinAtender = false;
static bool fileSystemSinAtender = false;
static pthread_t threadAntencionCpu;
static pthread_t threadAntencionKernel;
pthread_mutex_t mutexMemoriaData = PTHREAD_MUTEX_INITIALIZER;   //para controlar el flujo de atender-kernel/cpu/FS
pthread_mutex_t mutexListaDeSegmento = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexTamMemoriaActual = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char *argv[]) {

    imprimir_memoria();
    memoriaLogger = log_create(MEMORIA_LOG_UBICACION,MEMORIA_PROCESS_NAME,true,LOG_LEVEL_INFO);
    memoriaConfigInicial = config_create(argv[1]);
    memoriaConfig = memoria_config_initializer(memoriaConfigInicial);
    
    tamActualMemoria = memoria_config_get_tamanio_memoria(memoriaConfig); 
    memoriaPrincipal = malloc((size_t)tamActualMemoria);
    listaDeSegmentos = list_create();

    segCompartido = crear_segmento(memoria_config_get_tamanio_segmento_0(memoriaConfig));
    segmento_set_id(segCompartido,0);
    segmento_set_base(segCompartido,0);

    uint32_t limiteCompartido = memoria_config_get_tamanio_segmento_0(memoriaConfig) -1;
    segmento_set_limite(segCompartido, limiteCompartido);
    segmento_set_bit_validez(segCompartido, 1);
    list_add(listaDeSegmentos, segCompartido);    
    log_info(memoriaLogger,CYAN BOLD UNDERLINE "SE CREA EL SEGMENTO 0: "RESET YELLOW BOLD UNDERLINE"<%i>"RESET CYAN BOLD UNDERLINE" - BASE: "RESET YELLOW BOLD UNDERLINE"<%i>"RESET CYAN BOLD UNDERLINE" - TAMAÑO: "RESET YELLOW BOLD UNDERLINE"<%i>"RESET CYAN BOLD UNDERLINE" - LIMITE "RESET YELLOW BOLD UNDERLINE"<%i>", segmento_get_id(segCompartido), segmento_get_base(segCompartido), segmento_get_tamanio(segCompartido), segmento_get_limite(segCompartido));
    Segmento* segmentoUsuario = crear_segmento(tamActualMemoria - segmento_get_tamanio(segCompartido));
    segmento_set_base(segmentoUsuario,  segmento_get_limite(segCompartido) + 1 );
    segmento_set_limite(segmentoUsuario, segmento_get_base(segmentoUsuario) + segmento_get_tamanio(segmentoUsuario) - 1 );
    segmento_set_bit_validez(segmentoUsuario, 0);
    list_add_in_index(listaDeSegmentos,1, segmentoUsuario);
    log_info(memoriaLogger,GREEN BOLD UNDERLINE "SE CREA EL ESPACIO DE USUARIO: BASE: "RESET UNDERLINE YELLOW BOLD"<%i>"RESET GREEN BOLD UNDERLINE" - TAMAÑO: "RESET YELLOW UNDERLINE BOLD"<%i>"RESET GREEN BOLD UNDERLINE" - LIMITE: "RESET YELLOW UNDERLINE BOLD"<%i>", segmento_get_base(segmentoUsuario), segmento_get_tamanio(segmentoUsuario), segmento_get_limite(segmentoUsuario));
    restar_a_tamMemoriaActual(memoria_config_get_tamanio_segmento_0(memoriaConfig));
    log_info(memoriaLogger,BOLDRED "ALGORITMO DE ASIGNACION "RESET BOLDGREEN "<%s>", memoria_config_get_algoritmo_asignacion(memoriaConfig));
   //tabla_segmentos = estado_create();
    
    char* ipAddress = getIPAddress();
    printf(RED BOLD  "IP DE MEMORIA: %s\n", ipAddress);

    strcpy( memoriaConfig->IP_ESCUCHA , ipAddress);

    free(ipAddress);

    int serverMemoria = iniciar_servidor(memoria_config_get_ip_escucha(memoriaConfig), memoria_config_get_puerto_escucha(memoriaConfig) );
    log_info(memoriaLogger,ITALIC YELLOW "MODULO MEMORIA "RESET GREEN "ACTIVADO "RESET ITALIC YELLOW"ESPERANDO PARA RECIBIR A LOS OTROS MODULO\n");
    inicializar_memoria();
    aceptar_conexiones_memoria(serverMemoria);



}  

void* atender_conexiones_cpu(void* socket_ptr) {
    int socketCliente = *(int*)socket_ptr;
    log_info(memoriaLogger, "\e[1;92mSe acepta conexión de CPU en socket [%d]\e[0m", socketCliente);
    atender_peticiones_cpu(socketCliente);
    close(socketCliente); // Cerrar el socket cliente después de atender la conexión
    return NULL;
}

void* atender_conexiones_kernel(void* socket_ptr) {
    int socketCliente = *(int*)socket_ptr;
    log_info(memoriaLogger, "\e[1;92mSe acepta conexión de Kernel en socket [%d]\e[0m", socketCliente);
    memoria_config_set_socket_kernel(memoriaConfig,socketCliente);
    atender_peticiones_kernel(socketCliente);
    close(socketCliente); // Cerrar el socket cliente después de atender la conexión
    return NULL;
}

void* atender_conexiones_fileSystem(void* socket_ptr) {
    int socketCliente = *(int*)socket_ptr;
    log_info(memoriaLogger, "\e[1;92mSe acepta conexión de FileSystem en socket [%d]\e[0m", socketCliente);
    atender_peticiones_fileSystem(socketCliente);
    close(socketCliente); // Cerrar el socket cliente después de atender la conexión
    return NULL;
}

void aceptar_conexiones_memoria(const int socketEscucha) {
    struct sockaddr cliente = {0};
    socklen_t len = sizeof(cliente);
    int numClientesAtendidos = 0;

    while (numClientesAtendidos < 3) {
        const int clienteAceptado = accept(socketEscucha, &cliente, &len);

        if (clienteAceptado > -1) {
            uint8_t handshake = stream_recv_header(clienteAceptado);
            stream_recv_empty_buffer(clienteAceptado);
            if (handshake == HANDSHAKE_cpu) {
                pthread_t threadAtencion;
                pthread_create(&threadAtencion, NULL, atender_conexiones_cpu, &clienteAceptado);
                pthread_detach(threadAtencion);
                numClientesAtendidos++;
            } else if (handshake == HANDSHAKE_kernel) {
                pthread_t threadAtencion;
                pthread_create(&threadAtencion, NULL, atender_conexiones_kernel, &clienteAceptado);
                pthread_join(threadAtencion, NULL);
                numClientesAtendidos++;
            } else if (handshake == HANDSHAKE_fileSystem) {
                pthread_t threadAtencion;
                pthread_create(&threadAtencion, NULL, atender_conexiones_fileSystem, &clienteAceptado);
                pthread_detach(threadAtencion);
                numClientesAtendidos++;
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