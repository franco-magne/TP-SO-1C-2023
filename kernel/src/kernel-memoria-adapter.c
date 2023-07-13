#include <../include/kernel-memoria-adapter.h>

extern t_list* tablaGlobalDeSegmentos;
extern pthread_mutex_t mutexTablaGlobalSegmento;



void memoria_adapter_enviar_create_segment(t_pcb* pcbAIniciar, t_kernel_config* kernelConfig) {
    
    pthread_mutex_lock(&mutexTablaGlobalSegmento); 
    t_segmento* unSegmentoAEnviar = segmento_victima(tablaGlobalDeSegmentos);
    pthread_mutex_unlock(&mutexTablaGlobalSegmento);

    uint32_t id_de_segmento = segmento_get_id_de_segmento(unSegmentoAEnviar);
    uint32_t tamanio_de_segmento = segmento_get_tamanio_de_segmento(unSegmentoAEnviar);
    int pid = pcb_get_pid(pcbAIniciar);
        
    t_buffer* bufferNuevoSegmento = buffer_create();

    buffer_pack(bufferNuevoSegmento, &id_de_segmento, sizeof(id_de_segmento));
    buffer_pack(bufferNuevoSegmento, &tamanio_de_segmento, sizeof(tamanio_de_segmento));
    buffer_pack(bufferNuevoSegmento, &pid, sizeof(pid));

    stream_send_buffer(kernel_config_get_socket_memoria(kernelConfig), HEADER_create_segment ,bufferNuevoSegmento);

    buffer_destroy(bufferNuevoSegmento);
}

uint8_t memoria_adapter_recibir_create_segment(t_pcb* pcbAActualizar, t_kernel_config* kernelConfig, t_log* kernelLogger){
    
     
    pthread_mutex_lock(&mutexTablaGlobalSegmento); 
    t_segmento* unSegmentoAEnviar = segmento_victima(tablaGlobalDeSegmentos);
    uint32_t id_de_segmento = segmento_get_id_de_segmento(unSegmentoAEnviar);
    pthread_mutex_unlock(&mutexTablaGlobalSegmento);
    
    uint8_t headerMemoria = stream_recv_header(kernel_config_get_socket_memoria(kernelConfig));
    t_buffer* bufferTablaSegmentoActualizada = buffer_create();
    stream_recv_buffer(kernel_config_get_socket_memoria(kernelConfig), bufferTablaSegmentoActualizada);
    
    if(headerMemoria == HEADER_Compactacion){

        stream_send_empty_buffer(kernel_config_get_socket_file_system(kernelConfig), HEADER_Compactacion);
        log_info(kernelLogger,"TEST 1");
        uint8_t fileSystemResponse = stream_recv_header(kernel_config_get_socket_file_system(kernelConfig));
        stream_recv_empty_buffer(kernel_config_get_socket_file_system(kernelConfig));
        log_info(kernelLogger,"TEST 2");

        if(fileSystemResponse == HANDSHAKE_ok_continue){
            stream_send_empty_buffer(kernel_config_get_socket_memoria(kernelConfig), HANDSHAKE_ok_continue);
            headerMemoria = stream_recv_header(kernel_config_get_socket_memoria(kernelConfig));
            stream_recv_empty_buffer(kernel_config_get_socket_memoria(kernelConfig));
            if(headerMemoria == HEADER_Compactacion_finalizada){
                // recibir la tabla de segmento
                memoria_adapter_enviar_create_segment(pcbAActualizar,kernelConfig);
                memoria_adapter_recibir_create_segment(pcbAActualizar,kernelConfig,kernelLogger);
            }
        }

    }else if(headerMemoria == HANDSHAKE_ok_continue) {
        
        pthread_mutex_lock(&mutexTablaGlobalSegmento); 
        tablaGlobalDeSegmentos = buffer_unpack_segmento_list(bufferTablaSegmentoActualizada);
        
        t_segmento* aux = segmento_create(-1,-1);
        segmento_set_pid(aux, pcb_get_pid(pcbAActualizar));
        pcb_set_lista_de_segmentos(pcbAActualizar, list_filter_ok(tablaGlobalDeSegmentos,es_el_segmento_pid,aux));
        segmento_destroy(aux);

        int index = index_posicion_del_segmento_victima(tablaGlobalDeSegmentos, id_de_segmento,pcb_get_pid(pcbAActualizar));
        t_segmento* test = list_get(tablaGlobalDeSegmentos,index);
        log_info(kernelLogger,BOLD UNDERLINE CYAN  "PID <%i> : Segmento ID "RESET BOLD  BLUE "<%i>"BOLD UNDERLINE CYAN" : Base <%i> : Tamaño <%i>", test->pid, test->id_de_segmento, test->base_del_segmento, test->tamanio_de_segmento);
        log_info(kernelLogger, MAGENTA ITALIC "CANTIDAD DE SEGMENTOS TOTAL EN LA TABLA : <%i>", list_size(tablaGlobalDeSegmentos));
        pthread_mutex_unlock(&mutexTablaGlobalSegmento); 


        buffer_destroy(bufferTablaSegmentoActualizada);
         
        return HEADER_create_segment;
    
    } else if (headerMemoria == HEADER_memoria_insuficiente) {
        log_info(kernelLogger,BACKGROUND_RED GREEN ITALIC "PID: <%i> No pudo crear el segmento" RESET,  pcb_get_pid(pcbAActualizar));
        return HEADER_memoria_insuficiente;

    } else {
        log_error(kernelLogger, "Error al recibir buffer de la creacion");
        exit(-1);
    }


}


void memoria_adapter_enviar_delete_segment(t_pcb* pcbAIniciar, t_kernel_config* kernelConfig) {

    pthread_mutex_lock(&mutexTablaGlobalSegmento); 
    t_segmento* unSegmentoAEnviar = segmento_victima(tablaGlobalDeSegmentos);
    uint32_t id_de_segmento = segmento_get_id_de_segmento(unSegmentoAEnviar);
    pthread_mutex_unlock(&mutexTablaGlobalSegmento);

    uint32_t pid = pcb_get_pid(pcbAIniciar);

    t_buffer* bufferNuevoSegmento = buffer_create();

    buffer_pack(bufferNuevoSegmento, &id_de_segmento, sizeof(id_de_segmento));
    buffer_pack(bufferNuevoSegmento, &pid, sizeof(pid));


    stream_send_buffer(kernel_config_get_socket_memoria(kernelConfig), HEADER_delete_segment ,bufferNuevoSegmento);

    buffer_destroy(bufferNuevoSegmento);

}

void memoria_adapter_recibir_delete_segment(t_pcb* pcbAActualizar, t_kernel_config* kernelConfig, t_log* kernelLogger){
    
    pthread_mutex_lock(&mutexTablaGlobalSegmento); 
    t_segmento* unSegmentoEliminar = segmento_victima(tablaGlobalDeSegmentos);
    uint32_t id_de_segmento = segmento_get_id_de_segmento(unSegmentoEliminar);
    pthread_mutex_unlock(&mutexTablaGlobalSegmento);
   
    uint32_t pid = pcb_get_pid(pcbAActualizar);
    
    t_buffer* bufferTablaSegmentoActualizada = buffer_create();

    uint8_t headerMemoria = stream_recv_header(kernel_config_get_socket_memoria(kernelConfig));
    stream_recv_buffer(kernel_config_get_socket_memoria(kernelConfig), bufferTablaSegmentoActualizada);

     if (headerMemoria == HANDSHAKE_ok_continue) {
        log_info(kernelLogger,BOLD UNDERLINE CYAN "PID: <%i> - Eliminar Segmento - Id Segmento:"RESET BOLD BLUE" <%i>", pcb_get_pid(pcbAActualizar), id_de_segmento );
        
        pthread_mutex_lock(&mutexTablaGlobalSegmento); 
        tablaGlobalDeSegmentos = buffer_unpack_segmento_list(bufferTablaSegmentoActualizada);
        int index = index_posicion_del_segmento_victima(tablaGlobalDeSegmentos, id_de_segmento,pcb_get_pid(pcbAActualizar));
        
        t_segmento* aux = segmento_create(-1,-1);
        segmento_set_pid(aux, pcb_get_pid(pcbAActualizar));
        pcb_set_lista_de_segmentos(pcbAActualizar, list_filter_ok(tablaGlobalDeSegmentos,es_el_segmento_pid,aux));
        
        /*
        log_info(kernelLogger, "CANTIDAD DE SEGMENTOS TOTAL EN LA TABLA : <%i>", list_size(tablaGlobalDeSegmentos));
        
        printf("\n");
        printf("[");

        for(int i = 0; i<list_size(pcb_get_lista_de_segmentos(pcbAActualizar)); i++){
            t_segmento* aux1 = list_get(pcb_get_lista_de_segmentos(pcbAActualizar), i);
            printf("Segmento ID - <%i> - BASE - <%i> \t", aux1->id_de_segmento, aux1->base_del_segmento);
        }
        printf("]");
        printf("\n");
        */
        pthread_mutex_unlock(&mutexTablaGlobalSegmento); 
        
        buffer_destroy(bufferTablaSegmentoActualizada);


    } else if (headerMemoria == HEADER_error) {
        log_info(kernelLogger, "PID: <%i> No pudo eliminar el segmento",  pcb_get_pid(pcbAActualizar));
        return -1;
    } else {
        log_error(kernelLogger, "Error al recibir buffer de la creacion");
        exit(-1);
    }


}


void memoria_adapter_enviar_finalizar_proceso(t_pcb* pcb,  t_kernel_config* kernelConfig, t_log* kernelLogger, char* formaNotificar){

uint32_t pid = pcb_get_pid(pcb);

t_buffer* bufferProcesoFinalizado = buffer_create();

buffer_pack(bufferProcesoFinalizado, &pid, sizeof(pid));

stream_send_buffer(kernel_config_get_socket_memoria(kernelConfig), HEADER_proceso_terminado,bufferProcesoFinalizado);

uint8_t memoriaResponse =  stream_recv_header(kernel_config_get_socket_memoria(kernelConfig));
stream_recv_empty_buffer(kernel_config_get_socket_memoria(kernelConfig));

if(memoriaResponse == HANDSHAKE_ok_continue){
  
    log_info(kernelLogger, CYAN ITALIC BOLD "Finaliza el proceso <%i> - Motivo: <%s>" RESET CYAN ITALIC BOLD   , pid, formaNotificar);
}


}