#include <../include/kernel-memoria-adapter.h>


pthread_mutex_t mutexSocketMemoria;

void mem_adapter_crear_segmento(t_pcb* pcbAIniciar, t_kernel_config* kernelConfig, t_log* kernelLogger) {
    pthread_mutex_lock(&mutexSocketMemoria);
    uint32_t tamanio_de_segmento = pcb_get_tamanio_de_segmento(pcbAIniciar);
    uint32_t id_de_segmento = pcb_get_id_de_segmento(pcbAIniciar);

    t_buffer* bufferNuevoSegmento = buffer_create();
    buffer_pack(bufferNuevoSegmento, &id_de_segmento, sizeof(id_de_segmento));
    buffer_pack(bufferNuevoSegmento, &tamanio_de_segmento, sizeof(tamanio_de_segmento));
    stream_send_buffer(kernel_config_get_socket_memoria(kernelConfig),HEADER_create_segment , bufferNuevoSegmento);
    buffer_destroy(bufferNuevoSegmento);

    uint8_t memoriaResponse = stream_recv_header(kernel_config_get_socket_memoria(kernelConfig));

    if (memoriaResponse == HANDSHAKE_ok_continue) {
        //t_buffer* bufferTabla = buffer_create();
        //stream_recv_buffer(kernel_config_get_socket_memoria(kernelConfig), bufferTabla);

        //buffer_unpack(bufferTabla, &nroTabla, sizeof(nroTabla));
        //buffer_destroy(bufferTabla);
        log_info(kernelLogger, "Proceso <%d> : Pudo crear el segmento", pcb_get_pid(pcbAIniciar));
        //log_info(kernelLogger, "Proceso: %d - Tabla de página de primer nivel: %d", pcb_get_pid(pcbAIniciar), nroTabla);
    } else if (memoriaResponse == HEADER_error) {
        stream_recv_empty_buffer(kernel_config_get_socket_memoria(kernelConfig));
        pthread_mutex_unlock(&mutexSocketMemoria);
        return -1;
    } else {
        log_error(kernelLogger, "Error al recibir buffer de la creacion");
        exit(-1);
    }
    pthread_mutex_unlock(&mutexSocketMemoria);
    
}