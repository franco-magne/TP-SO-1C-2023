
#include <../include/cpu-memoria-adapter.h>


extern t_cpu_config *cpuConfig;
extern t_log *cpuLogger;


bool es_el_segmento_id(t_segmento* unSegmento, t_segmento* otroSegmento){
    return unSegmento->id_de_segmento == otroSegmento->id_de_segmento;
} 




t_segmento* obtener_base_segmento_num_segmento(t_list* listaDeSegmento, uint32_t num_segmento){
      t_segmento* aux = segmento_create(num_segmento, -1);
    int index = list_get_index(listaDeSegmento, es_el_segmento_id, aux);

    uint32_t base_del_segmento;
    if (index >= 0) {
        t_segmento* segmentoVictima = list_get(listaDeSegmento, index);
        free(aux);
        return segmentoVictima;
    } else {
        return aux;
    }

   
}




//seria la implementacion segmentation
static uint32_t cpu_chequeo_base(int toSocket, uint32_t base, uint32_t desplazamiento_segmento ,  t_header requestHeader) {
    t_buffer *requestBuffer = buffer_create();
    
    buffer_pack(requestBuffer, &base, sizeof(base));
    buffer_pack(requestBuffer, &desplazamiento_segmento, sizeof(desplazamiento_segmento));

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
uint32_t cpu_mmu(int toSocket, uint32_t direccionLogica, t_list* tablaDeSegmento, uint32_t pid) {
    int tamanioMaximoSegmento = cpu_config_get_tamanio_maximo_segmento(cpuConfig);
    uint32_t num_segmento  = floor(direccionLogica / tamanioMaximoSegmento);
    uint32_t desplazamiento_segmento = direccionLogica % tamanioMaximoSegmento;
    
    t_segmento* segmento = obtener_base_segmento_num_segmento(tablaDeSegmento, num_segmento); // MODIFICARLA
    uint32_t baseDelSegmento = cpu_chequeo_base(toSocket, segmento->base_del_segmento, desplazamiento_segmento, HEADER_chequeo_DF);
    
    if(baseDelSegmento == 0)
    log_info(cpuLogger,"PID: <%i> - Error SEG_FAULT- Segmento: <%i> - Offset: <%i> - Tamaño: <%i>",pid, num_segmento, desplazamiento_segmento, segmento->tamanio_de_segmento);
    

    return baseDelSegmento;
    
}

// Write
void cpu_escribir_en_memoria(int toSocket, uint32_t dirFisica, char* contenidoAEscribir, t_cpu_pcb* pcb) {
    uint32_t baseDelSegmentoAEnviar = dirFisica;
    t_buffer *buffer = buffer_create();
    buffer_pack(buffer, &baseDelSegmentoAEnviar, sizeof(baseDelSegmentoAEnviar)); //  BASE 
    buffer_pack_string(buffer, contenidoAEscribir);
    stream_send_buffer(toSocket, HEADER_move_out, buffer);
    buffer_destroy(buffer);

}

// Read
char* cpu_leer_en_memoria( int toSocket, uint32_t dirFisica, t_cpu_pcb* pcb) {
    uint32_t baseDelSegmentoAEnviar = dirFisica;
    t_buffer *requestBuffer = buffer_create();
    buffer_pack(requestBuffer, &baseDelSegmentoAEnviar, sizeof(baseDelSegmentoAEnviar));
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