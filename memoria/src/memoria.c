#include <../include/memoria.h>


t_log *memoriaLogger;
static t_config *memoriaConfigInicial;
t_memoria_config* memoriaConfig;
Segmento* segCompartido;
t_list* listaDeSegmentos;
void* memoriaPrincipal;

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

    int tamanioMP = (int) memoria_config_get_tamanio_memoria(memoriaConfig); 
    memoriaPrincipal = malloc(tamanioMP);
    listaDeSegmentos = list_create();
    segCompartido = crear_segmento(memoria_config_get_tamanio_segmento_0(memoriaConfig));
    segmento_set_id(segCompartido,0);
    segmento_set_base(segCompartido,0);
    segmento_set_limite(segCompartido, memoria_config_get_tamanio_segmento_0(memoriaConfig));
    segmento_set_bit_validez(segCompartido, 1);
    list_add(listaDeSegmentos, segCompartido);
    log_info(memoriaLogger,"Crear Segmento 0: <%i> - Base: <%i> - TAMAÑO: <%i> - LIMITE <%i>", segmento_get_id(segCompartido), segmento_get_base(segCompartido), segmento_get_tamanio(segCompartido), segmento_get_limite(segCompartido));
    Segmento* segmentoUsuario = crear_segmento(tamanioMP - segmento_get_tamanio(segCompartido));
    segmento_set_base(segmentoUsuario,  segmento_get_limite(segCompartido) );
    segmento_set_limite(segmentoUsuario, segmento_get_base(segmentoUsuario) + segmento_get_tamanio(segmentoUsuario));
    segmento_set_bit_validez(segmentoUsuario, 0);
    list_add_in_index(listaDeSegmentos,1, segmentoUsuario);
    log_info(memoriaLogger,"Crear Segmento 1: <%i> - Base: <%i> - TAMAÑO: <%i> - LIMITE <%i>", segmento_get_id(segmentoUsuario), segmento_get_base(segmentoUsuario), segmento_get_tamanio(segmentoUsuario), segmento_get_limite(segmentoUsuario));

   //tabla_segmentos = estado_create();

    int serverMemoria = iniciar_servidor(memoria_config_get_ip_escucha(memoriaConfig), memoria_config_get_puerto_escucha(memoriaConfig) );
    log_info(memoriaLogger,"Servidor memoria listo para recibir al modulo\n");
    inicializar_memoria();
    aceptar_conexiones_memoria(serverMemoria);

}  
/*
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
        pthread_create(&threadAntencionCpu, NULL, atender_peticiones_cpu, socketCliente);
        pthread_detach(threadAntencionCpu);
        cpuSinAtender = false;
    } 

    else if (handshake == HANDSHAKE_kernel) {
        log_info(memoriaLogger, "\e[1;92mSe acepta conexión de Kernel en socket [%d]\e[0m", socketCliente);
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
    }
    else {
        log_error(memoriaLogger, "Error al recibir handshake de cliente: %s", strerror(errno));
        exit(-1);
    }
    return;
}
*/
/*
int main() {

   memoriaLogger = log_create(MEMORIA_LOG_UBICACION,MEMORIA_PROCESS_NAME,true,LOG_LEVEL_INFO);
   memoriaConfigInicial = config_create(MEMORIA_CONFIG_UBICACION);
   memoriaConfig = memoria_config_initializer(memoriaConfigInicial);
   inicializar_memoria();
   
   int serverMemoria = iniciar_servidor(memoria_config_get_ip_escucha(memoriaConfig), memoria_config_get_puerto_escucha(memoriaConfig) );
   log_info(memoriaLogger,"Servidor memoria listo para recibir al modulo\n");
   aceptar_conexiones_memoria(serverMemoria);
   

}  

*/
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
                memoria_config_set_socket_kernel(memoriaConfig,clienteAceptado);
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
