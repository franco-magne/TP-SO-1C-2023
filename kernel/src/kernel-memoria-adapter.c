#include <../include/kernel-memoria-adapter.h>



void mem_adapter_crear_segmento(t_pcb* pcbAIniciar, t_kernel_config* kernelConfig, t_log* kernelLogger) {
    uint32_t tamanio_de_segmento = pcb_get_tamanio_de_segmento(pcbAIniciar);
    uint32_t id_de_segmento = pcb_get_id_de_segmento(pcbAIniciar);
    
    log_info(kernelLogger, "1- socket de kernel: %i", kernel_config_get_socket_memoria(kernelConfig));

    t_buffer* bufferNuevoSegmento = buffer_create();
    buffer_pack(bufferNuevoSegmento, &id_de_segmento, sizeof(id_de_segmento));
    buffer_pack(bufferNuevoSegmento, &tamanio_de_segmento, sizeof(tamanio_de_segmento));
    stream_send_buffer(kernel_config_get_socket_memoria(kernelConfig), HEADER_create_segment ,bufferNuevoSegmento);

    log_info(kernelLogger, "2- socket de kernel: %i", kernel_config_get_socket_memoria(kernelConfig));
     sleep(5);
     uint8_t headerMemoria = stream_recv_header(kernel_config_get_socket_memoria(kernelConfig));
    log_info(kernelLogger, "valor %i", headerMemoria);
    if (headerMemoria == HANDSHAKE_ok_continue) {
        //t_buffer* bufferTabla = buffer_create();
        //stream_recv_buffer(kernel_config_get_socket_memoria(kernelConfig), bufferTabla);

        //buffer_unpack(bufferTabla, &nroTabla, sizeof(nroTabla));
        //buffer_destroy(bufferTabla);
        log_info(kernelLogger, "Proceso <%d> : Pudo crear el segmento", pcb_get_pid(pcbAIniciar));
        //log_info(kernelLogger, "Proceso: %d - Tabla de página de primer nivel: %d", pcb_get_pid(pcbAIniciar), nroTabla);
    } else if (headerMemoria == HEADER_error) {
        log_info(kernelLogger, "valor %i", headerMemoria);
        //pthread_mutex_unlock(&mutexSocketMemoria);
        //return -1;
    } else {
        log_error(kernelLogger, "Error al recibir buffer de la creacion");
        exit(-1);
    }
    buffer_destroy(bufferNuevoSegmento);

}