
#include <../include/cpu-memoria-adapter.h>


extern t_cpu_config *cpuConfig;
extern t_log *cpuLogger;


bool es_el_segmento_id(t_segmento* unSegmento, t_segmento* otroSegmento){
    return unSegmento->id_de_segmento == otroSegmento->id_de_segmento;
} 

uint32_t obtener_base_segmento_num_segmento(t_list* listaDeSegmento, uint32_t num_segmento){
    t_segmento* aux = malloc(sizeof(*aux));
    aux->id_de_segmento = num_segmento;

    int index = list_get_index(listaDeSegmento,es_el_segmento_id, aux);
    t_segmento* segmentoVictima = list_get(listaDeSegmento, index);

    free(aux);

    return segmentoVictima->base_del_segmento ;

}




//seria la implementacion segmentation
static uint32_t cpu_chequeo_base(int toSocket, uint32_t base, uint32_t desplazamiento_segmento ,  t_header requestHeader) {
    t_buffer *requestBuffer = buffer_create();
    
    buffer_pack(requestBuffer, &base, sizeof(base));
    buffer_pack(requestBuffer, &desplazamiento_segmento, sizeof(desplazamiento_segmento));

    stream_send_buffer(toSocket, requestHeader, requestBuffer); //---->>>> requestHeader == HEADER_chequeo_DF
    buffer_destroy(requestBuffer);
    uint8_t responseHeader = stream_recv_header(toSocket);  //---->>>> Aca recibo el HEADER_chequeo_DF o el HEADER_Segmentation_fault
    if (responseHeader != requestHeader) { //si me mandan un HEADER_SegFault
        //log_error(cpuLogger, "PID: <%i> - Error SEG_FAULT- Segmento_id: <%i> -Offset: <%i> - Tamaño: <TAMANIO>", pid, id_segmento, desplazamiento_segmento, tamanio);
        //avisarle a Kernel que lo mande a EXIT x SegFault
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
uint32_t cpu_mmu(int toSocket, uint32_t direccionLogica, t_list* tablaDeSegmento) {
    int tamanioMaximoSegmento = cpu_config_get_tamanio_maximo_segmento(cpuConfig);

    uint32_t num_segmento  = floor(direccionLogica / tamanioMaximoSegmento);

    uint32_t baseDelSegmento = obtener_base_segmento_num_segmento(tablaDeSegmento, num_segmento);

    printf("Numero de segmento <%i> - Base de segmento <%i> - Tamaño maximo <%i> - DireccionLogica <%i>", num_segmento, baseDelSegmento , tamanioMaximoSegmento,direccionLogica);
    uint32_t desplazamiento_segmento = direccionLogica % tamanioMaximoSegmento;
    
    baseDelSegmento = cpu_chequeo_base(toSocket, baseDelSegmento, desplazamiento_segmento, HEADER_chequeo_DF);
    
    return baseDelSegmento;
    
}

// Write
void cpu_escribir_en_memoria(int toSocket, uint32_t direccionAEscribir, char* contenidoAEscribir, t_cpu_pcb* pcb) {
    uint32_t baseDelSegmentoAEnviar = cpu_mmu(toSocket, direccionAEscribir,cpu_pcb_get_tabla_de_segmento(pcb));
    t_buffer *buffer = buffer_create();
    buffer_pack(buffer, &baseDelSegmentoAEnviar, sizeof(baseDelSegmentoAEnviar)); //  BASE 
    buffer_pack_string(buffer, contenidoAEscribir);
    stream_send_buffer(toSocket, HEADER_move_out, buffer);
    buffer_destroy(buffer);
}

// Read
char* cpu_leer_en_memoria( int toSocket, uint32_t direccionALeer, t_cpu_pcb* pcb) {
    uint32_t baseDelSegmentoAEnviar = cpu_mmu(toSocket, direccionALeer, cpu_pcb_get_tabla_de_segmento(pcb));
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