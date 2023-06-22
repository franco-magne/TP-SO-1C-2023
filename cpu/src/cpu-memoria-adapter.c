
#include <../include/cpu-memoria-adapter.h>


extern t_cpu_config *cpuConfig;
extern t_log *cpuLogger;

static uint32_t cpu_solicitar_a_memoria(int toSocket, double num_segmento, uint32_t pid, uint32_t desplazamiento_segmento ,  t_header requestHeader) {
    t_buffer *requestBuffer = buffer_create();
    
    buffer_pack(requestBuffer, &num_segmento, sizeof(num_segmento));
    buffer_pack(requestBuffer, &pid, sizeof(pid));
    buffer_pack(requestBuffer, &desplazamiento_segmento, sizeof(desplazamiento_segmento));

    stream_send_buffer(toSocket, requestHeader, requestBuffer);
    buffer_destroy(requestBuffer);
    uint8_t responseHeader = stream_recv_header(toSocket);
    if (responseHeader != requestHeader) {
        log_error(cpuLogger, "Error al realizar una solicitud de memoria");
        exit(-1);
    }
    t_buffer *responseBuffer = buffer_create();
    stream_recv_buffer(toSocket, responseBuffer);
    uint32_t requestRetVal = -1;
    buffer_unpack(responseBuffer, &requestRetVal, sizeof(requestRetVal));
    buffer_destroy(responseBuffer);
    return requestRetVal;
}

static uint32_t cpu_obtener_marco(int toSocket, uint32_t direccionLogica, uint32_t pid) {
    int tamanioMaximoSegmento = cpu_config_get_tamanio_maximo_segmento(cpuConfig);

    double num_segmento  = floor(direccionLogica / tamanioMaximoSegmento);
    uint32_t desplazamiento_segmento = direccionLogica % tamanioMaximoSegmento;
    

    int marco = cpu_solicitar_a_memoria(toSocket, num_segmento, pid,desplazamiento_segmento, HEADER_marco);
    

    return marco;
}

// Write
void cpu_escribir_en_memoria(int toSocket, uint32_t direccionAEscribir, char* contenidoAEscribir, uint32_t pid) {
    int marco = cpu_obtener_marco(toSocket, direccionAEscribir, pid);
    t_buffer *buffer = buffer_create();
    uint32_t marcoSend = marco;
    buffer_pack(buffer, &marcoSend, sizeof(marcoSend));
    buffer_pack(buffer, &pid, sizeof(pid));
    buffer_pack_string(buffer, contenidoAEscribir);
    stream_send_buffer(toSocket, HEADER_move_out, buffer);
    buffer_destroy(buffer);
}

// Read
char* cpu_leer_en_memoria( int toSocket, uint32_t direccionALeer, uint32_t pid ) {
    int marco = cpu_obtener_marco(toSocket, direccionALeer, pid);
    t_buffer *requestBuffer = buffer_create();
    uint32_t marcoAEnviar = marco;
    buffer_pack(requestBuffer, &marcoAEnviar, sizeof(marcoAEnviar));
    buffer_pack(requestBuffer,&pid,sizeof(pid));
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