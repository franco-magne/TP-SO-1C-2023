#include <../include/kernel-memoria-adapter.h>




void instruccion_create_segment(t_pcb* pcbAIniciar, t_kernel_config* kernelConfig, t_log* kernelLogger) {
    uint32_t id_de_segmento_search = pcb_get_id_de_segmento(pcbAIniciar);
    
    log_info(kernelLogger, "id <%i>", id_de_segmento_search);

    //t_list* listaDeSegmentoPcb = pcb_get_lista_de_segmentos(pcbAIniciar);
    t_segmento* unSegmentoAEnviar = enviar_segmento_a_memoria(pcbAIniciar,id_de_segmento_search);
   
    uint32_t id_de_segmento = segmento_get_id_de_segmento(unSegmentoAEnviar);
    uint32_t tamanio_de_segmento = segmento_get_tamanio_de_segmento(unSegmentoAEnviar);

    t_buffer* bufferNuevoSegmento = buffer_create();

    buffer_pack(bufferNuevoSegmento, &id_de_segmento, sizeof(id_de_segmento));
    buffer_pack(bufferNuevoSegmento, &tamanio_de_segmento, sizeof(tamanio_de_segmento));

    stream_send_buffer(kernel_config_get_socket_memoria(kernelConfig), HEADER_create_segment ,bufferNuevoSegmento);


     stream_recv_empty_buffer(kernel_config_get_socket_memoria(kernelConfig));
     uint8_t headerMemoria = stream_recv_header(kernel_config_get_socket_memoria(kernelConfig));

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

void instruccion_delete_segment(t_pcb* pcbAIniciar, t_kernel_config* kernelConfig, t_log* kernelLogger) {

}