#include <../include/cpu-memoria-adapter.h>


extern t_cpu_config *cpuConfig;
extern t_log *cpuLogger;


bool es_el_segmento_id(t_segmento* unSegmento, t_segmento* otroSegmento) {
    return unSegmento->id_de_segmento == otroSegmento->id_de_segmento;
} 

t_segmento* obtener_base_segmento_num_segmento(t_list* listaDeSegmento, uint32_t num_segmento) {
    
    t_segmento* aux = segmento_create(num_segmento, -1);
    int index = list_get_index(listaDeSegmento, es_el_segmento_id, aux);

    uint32_t base_del_segmento;
    if (index >= 0) {
        t_segmento* segmentoVictima = list_get(listaDeSegmento, index);
        free(aux);
        return segmentoVictima;
    } else {
        aux->base_del_segmento = 0;
        return aux;
    }   
}

//seria la implementacion segmentation
static uint32_t cpu_chequeo_base(int toSocket, uint32_t base, uint32_t desplazamiento_segmento ,  t_header requestHeader, uint32_t cantidadByte) {
    
    t_buffer *requestBuffer = buffer_create();
    
    buffer_pack(requestBuffer, &base, sizeof(base));
    buffer_pack(requestBuffer, &desplazamiento_segmento, sizeof(desplazamiento_segmento));
    buffer_pack(requestBuffer, &cantidadByte, sizeof(cantidadByte));


    stream_send_buffer(toSocket, requestHeader, requestBuffer); 
    buffer_destroy(requestBuffer);
    uint8_t responseHeader = stream_recv_header(toSocket);  

    if (responseHeader != requestHeader) { 
        exit(-1);
    } 

    t_buffer *responseBuffer = buffer_create();
    stream_recv_buffer(toSocket, responseBuffer);
    uint32_t requestRetVal = -1;
    buffer_unpack(responseBuffer, &requestRetVal, sizeof(requestRetVal));
    buffer_destroy(responseBuffer);
    
    return requestRetVal;
}

//MMU
void cpu_mmu(int toSocket, uint32_t direccionLogica, t_list* tablaDeSegmento, t_cpu_pcb* pcb , uint32_t cantidadByte) {

    int tamanioMaximoSegmento = cpu_config_get_tamanio_maximo_segmento(cpuConfig);
    uint32_t num_segmento  = floor(direccionLogica / tamanioMaximoSegmento);
    uint32_t desplazamiento_segmento = direccionLogica % tamanioMaximoSegmento;
    t_segmento* segmento = obtener_base_segmento_num_segmento(tablaDeSegmento, num_segmento); // MODIFICARLA
   
    if(segmento->base_del_segmento == 0){
            uint32_t tamanioSegmento0 = cpu_chequeo_base(toSocket, segmento->base_del_segmento , desplazamiento_segmento, HEADER_chequeo_DF, cantidadByte);
            segmento->tamanio_de_segmento = tamanioSegmento0;
    }


    if((desplazamiento_segmento + cantidadByte) > segmento->tamanio_de_segmento){
            log_info(cpuLogger, BACKGROUND_RED BOLD YELLOW "PID: <%i> - Error SEG_FAULT- Segmento: <%i> - Offset: <%i> - Tamaño: <%i>" RESET, cpu_pcb_get_pid(pcb), num_segmento, desplazamiento_segmento, segmento->tamanio_de_segmento);
            segmento->base_del_segmento = -1;
    }
    
    cpu_pcb_set_cantidad_byte(pcb,cantidadByte); // cantidad_byte
    cpu_pcb_set_base_direccion_fisica(pcb,segmento->base_del_segmento); //base_direccion_fisica
    cpu_pcb_set_desplazamiento_segmento(pcb,desplazamiento_segmento); //oka
}

// Write
void cpu_escribir_en_memoria(int toSocket,char* contenidoAEscribir, t_cpu_pcb* pcb) {

    uint32_t baseDireccionFisica = cpu_pcb_get_base_direccion_fisica(pcb);
    uint32_t desplazamientoSegmento = cpu_pcb_get_desplazamiento_segmento(pcb);
    uint32_t cantidadByte = cpu_pcb_get_cantidad_byte(pcb);
    t_buffer *buffer = buffer_create();

    buffer_pack(buffer, &baseDireccionFisica, sizeof(baseDireccionFisica)); 
    buffer_pack(buffer, &desplazamientoSegmento, sizeof(desplazamientoSegmento));
    buffer_pack(buffer, &cantidadByte, sizeof(cantidadByte));

    buffer_pack_string(buffer, contenidoAEscribir);
    stream_send_buffer(toSocket, HEADER_move_out, buffer);
    buffer_destroy(buffer);
}

// Read
char* cpu_leer_en_memoria( int toSocket,t_cpu_pcb* pcb) {
    
    t_buffer *requestBuffer = buffer_create();

    uint32_t baseDireccionFisica = cpu_pcb_get_base_direccion_fisica(pcb);
    uint32_t desplazamientoSegmento = cpu_pcb_get_desplazamiento_segmento(pcb);
    uint32_t cantidadByte = cpu_pcb_get_cantidad_byte(pcb);
    
    buffer_pack(requestBuffer, &baseDireccionFisica, sizeof(baseDireccionFisica)); 
    buffer_pack(requestBuffer, &desplazamientoSegmento, sizeof(desplazamientoSegmento));
    buffer_pack(requestBuffer, &cantidadByte, sizeof(cantidadByte));

    stream_send_buffer(toSocket, HEADER_move_in, requestBuffer);
    buffer_destroy(requestBuffer);

    uint32_t responseHeader = stream_recv_header(toSocket);
    if (responseHeader != HEADER_move_in) 
    {
        log_error(cpuLogger, "Error al leer en memoria");
        exit(-1);
    }

    t_buffer *responseBuffer = buffer_create();
    stream_recv_buffer(toSocket, responseBuffer);
    char* contenidoLeido = buffer_unpack_string(responseBuffer);
    buffer_destroy(responseBuffer);

    return contenidoLeido;
}