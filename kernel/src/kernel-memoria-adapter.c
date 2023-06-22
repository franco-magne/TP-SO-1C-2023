#include <../include/kernel-memoria-adapter.h>

void instruccion_iniciar_proceso(t_pcb* pcbAIniciar, t_kernel_config* kernelConfig, t_log* kernelLogger){
    int pid = -1; //pcb_get_pid(pcbAIniciar);   ponemos -1 xq es el pid generico cuando creamos el segCompartido
    uint32_t tam_segmento;
    uint32_t id_segmento;
    t_buffer* bufferNuevoProceso = buffer_create();

    buffer_pack(bufferNuevoProceso, &pid, sizeof(pid));
    stream_send_buffer(kernel_config_get_socket_memoria(kernelConfig), HEADER_iniciar_proceso, bufferNuevoProceso);

    buffer_destroy(bufferNuevoProceso);
    t_buffer* bufferResponse = buffer_create();
    stream_recv_buffer(kernel_config_get_socket_memoria(kernelConfig), bufferResponse);
    buffer_unpack(bufferResponse, &id_segmento, sizeof(id_segmento));
    buffer_unpack(bufferResponse, &tam_segmento, sizeof(tam_segmento));

    t_segmento* segmentoCero = segmento_create(id_segmento, tam_segmento);
    list_add(pcbAIniciar->listaDeSegmento, segmentoCero);

    buffer_destroy(bufferNuevoProceso);
}


void instruccion_create_segment(t_pcb* pcbAIniciar, t_kernel_config* kernelConfig, t_log* kernelLogger) {
    
    t_segmento* unSegmentoAEnviar = enviar_segmento_a_memoria(pcbAIniciar, HEADER_create_segment);


    uint32_t id_de_segmento = segmento_get_id_de_segmento(unSegmentoAEnviar);
    uint32_t tamanio_de_segmento = segmento_get_tamanio_de_segmento(unSegmentoAEnviar);
    int pid = pcb_get_pid(pcbAIniciar);

    t_buffer* bufferNuevoSegmento = buffer_create();

    buffer_pack(bufferNuevoSegmento, &id_de_segmento, sizeof(id_de_segmento));
    buffer_pack(bufferNuevoSegmento, &tamanio_de_segmento, sizeof(tamanio_de_segmento));
    buffer_pack(bufferNuevoSegmento, &pid, sizeof(pid));

    stream_send_buffer(kernel_config_get_socket_memoria(kernelConfig), HEADER_create_segment ,bufferNuevoSegmento);

    uint8_t headerMemoria = stream_recv_header(kernel_config_get_socket_memoria(kernelConfig));
    stream_recv_empty_buffer(kernel_config_get_socket_memoria(kernelConfig));

    if (headerMemoria == HANDSHAKE_ok_continue) {

        log_info(kernelLogger, "PID: <%i> - Crear Segmento - Id: <%i> - Tamaño: <%i>", pcb_get_pid(pcbAIniciar), id_de_segmento, tamanio_de_segmento);

    } else if (headerMemoria == HEADER_error) {
        log_info(kernelLogger, "PID: <%i> No pudo crear el segmento",  pcb_get_pid(pcbAIniciar));
        return -1;
    } else {
        log_error(kernelLogger, "Error al recibir buffer de la creacion");
        exit(-1);
    }
    buffer_destroy(bufferNuevoSegmento);

}

void instruccion_delete_segment(t_pcb* pcbAIniciar, t_kernel_config* kernelConfig, t_log* kernelLogger) {

    t_segmento* unSegmentoAEnviar = enviar_segmento_a_memoria(pcbAIniciar, HEADER_delete_segment);

    uint32_t id_de_segmento = segmento_get_id_de_segmento(unSegmentoAEnviar);
    int pid = pcb_get_pid(pcbAIniciar);

    t_buffer* bufferNuevoSegmento = buffer_create();
    t_buffer* bufferResponse = buffer_create();

    buffer_pack(bufferNuevoSegmento, &id_de_segmento, sizeof(id_de_segmento));
    buffer_pack(bufferNuevoSegmento, &pid, sizeof(pid));

    stream_send_buffer(kernel_config_get_socket_memoria(kernelConfig), HEADER_delete_segment ,bufferNuevoSegmento);

    
    stream_recv_buffer(kernel_config_get_socket_memoria(kernelConfig), bufferResponse);
    uint32_t header = stream_recv_header(kernel_config_get_socket_memoria(kernelConfig));

     if (header == HEADER_tabla_segmentos) {

        log_info(kernelLogger, "PID: <%i> - Eliminar Segmento - Id Segmento: <%i>", pcb_get_pid(pcbAIniciar), id_de_segmento);

    } else if (header == HEADER_error) {
        log_info(kernelLogger, "PID: <%i> No pudo crear el segmento",  pcb_get_pid(pcbAIniciar));
        return -1;
    } else {
        log_error(header, "Error al recibir buffer de la creacion");
        exit(-1);
    }
    buffer_destroy(bufferNuevoSegmento);
}