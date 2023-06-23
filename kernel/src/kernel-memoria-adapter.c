#include <../include/kernel-memoria-adapter.h>




void memoria_adapter_enviar_create_segment(t_pcb* pcbAIniciar, t_kernel_config* kernelConfig) {
    
    t_segmento* unSegmentoAEnviar = segmento_victima(pcbAIniciar);

    uint32_t id_de_segmento = segmento_get_id_de_segmento(unSegmentoAEnviar);
    uint32_t tamanio_de_segmento = segmento_get_tamanio_de_segmento(unSegmentoAEnviar);
    uint32_t pid = pcb_get_pid(pcbAIniciar);
    t_buffer* bufferNuevoSegmento = buffer_create();

    buffer_pack(bufferNuevoSegmento, &id_de_segmento, sizeof(id_de_segmento));
    buffer_pack(bufferNuevoSegmento, &tamanio_de_segmento, sizeof(tamanio_de_segmento));
    buffer_pack(bufferNuevoSegmento, &pid, sizeof(pid));

    stream_send_buffer(kernel_config_get_socket_memoria(kernelConfig), HEADER_create_segment ,bufferNuevoSegmento);

}

uint8_t memoria_adapter_recibir_create_segment(t_pcb* pcbAActualizar, t_kernel_config* kernelConfig, t_log* kernelLogger){
    
     
    t_segmento* unSegmentoAEnviar = segmento_victima(pcbAActualizar);
    uint32_t id_de_segmento = segmento_get_id_de_segmento(unSegmentoAEnviar);

    uint8_t headerMemoria = stream_recv_header(kernel_config_get_socket_memoria(kernelConfig));
    stream_recv_empty_buffer(kernel_config_get_socket_memoria(kernelConfig));

    if (headerMemoria == HANDSHAKE_ok_continue) {

        log_info(kernelLogger, "PID: <%i> - Creo el Segmento - Id: <%i>", pcb_get_pid(pcbAActualizar), id_de_segmento);
        modificar_victima_lista_segmento(pcbAActualizar, id_de_segmento, false);
        return HEADER_create_segment;
    
    } else if (headerMemoria == HEADER_memoria_insuficiente) {
        log_info(kernelLogger, "PID: <%i> No pudo crear el segmento",  pcb_get_pid(pcbAActualizar));
        return HEADER_memoria_insuficiente;

    } else {
        log_error(kernelLogger, "Error al recibir buffer de la creacion");
        exit(-1);
    }


}


void memoria_adapter_enviar_delete_segment(t_pcb* pcbAIniciar, t_kernel_config* kernelConfig) {

    t_segmento* unSegmentoAEnviar = segmento_victima(pcbAIniciar);

    uint32_t id_de_segmento = segmento_get_id_de_segmento(unSegmentoAEnviar);
    uint32_t pid = pcb_get_pid(pcbAIniciar);

    t_buffer* bufferNuevoSegmento = buffer_create();

    buffer_pack(bufferNuevoSegmento, &id_de_segmento, sizeof(id_de_segmento));
    buffer_pack(bufferNuevoSegmento, &pid, sizeof(pid));


    stream_send_buffer(kernel_config_get_socket_memoria(kernelConfig), HEADER_delete_segment ,bufferNuevoSegmento);

    buffer_destroy(bufferNuevoSegmento);

}

void memoria_adapter_recibir_delete_segment(t_pcb* pcbAActualizar, t_kernel_config* kernelConfig, t_log* kernelLogger){



    uint8_t headerMemoria = stream_recv_header(kernel_config_get_socket_memoria(kernelConfig));

    stream_recv_empty_buffer(kernel_config_get_socket_memoria(kernelConfig));

     if (headerMemoria == HANDSHAKE_ok_continue) {
        t_segmento* unSegmento = remover_segmento_victima_lista(pcbAActualizar);
        log_info(kernelLogger, "PID: <%i> - Eliminar Segmento - Id Segmento: <%i>", pcb_get_pid(pcbAActualizar), segmento_get_id_de_segmento(unSegmento) );


    } else if (headerMemoria == HEADER_error) {
        log_info(kernelLogger, "PID: <%i> No pudo eliminar el segmento",  pcb_get_pid(pcbAActualizar));
        return -1;
    } else {
        log_error(kernelLogger, "Error al recibir buffer de la creacion");
        exit(-1);
    }


}