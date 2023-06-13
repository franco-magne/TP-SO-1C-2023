
#include <../include/cpu-memoria-adapter.h>


extern t_cpu_config *cpuConfig;
extern t_log *cpuLogger;

static uint32_t cpu_solicitar_a_memoria(int toSocket, uint32_t numeroDeTabla, uint32_t entradaDeTabla, t_header requestHeader) {
    t_buffer *requestBuffer = buffer_create();
    buffer_pack(requestBuffer, &numeroDeTabla, sizeof(numeroDeTabla));
    buffer_pack(requestBuffer, &entradaDeTabla, sizeof(entradaDeTabla));
    stream_send_buffer(toSocket, requestHeader, requestBuffer);
    buffer_destroy(requestBuffer);
    uint32_t responseHeader = stream_recv_header(toSocket);
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

static uint32_t cpu_obtener_marco(int toSocket, uint32_t direccionLogica) {
    int tamanioMaximoSegmento = cpu_config_get_tamanio_maximo_segmento(cpuConfig);

    uint32_t num_segmento  = floor(direccionLogica / tamanioMaximoSegmento);
    uint32_t desplazamiento_segmento = direccionLogica % tamanioMaximoSegmento;
   

    int marco = cpu_solicitar_a_memoria(toSocket, num_segmento, desplazamiento_segmento, HEADER_marco);
    

    return marco + desplazamiento_segmento;
}

void cpu_escribir_en_memoria(int toSocket, uint32_t direccionAEscribir, uint32_t contenidoAEscribir) {
    int marco = cpu_obtener_marco(toSocket, direccionAEscribir);
    t_buffer *buffer = buffer_create();
    uint32_t marcoSend = marco;
    buffer_pack(buffer, &marcoSend, sizeof(marcoSend));
    buffer_pack(buffer, &contenidoAEscribir, sizeof(contenidoAEscribir));
//   stream_send_buffer(toSocket, HEADER_write, buffer);
    buffer_destroy(buffer);
}

uint32_t cpu_leer_en_memoria( int toSocket, uint32_t direccionALeer) {
    int marco = cpu_obtener_marco(toSocket, direccionALeer);
    t_buffer *requestBuffer = buffer_create();
    uint32_t marcoAEnviar = marco;
    buffer_pack(requestBuffer, &marcoAEnviar, sizeof(marcoAEnviar));
   // stream_send_buffer(toSocket, HEADER_read, requestBuffer);
    buffer_destroy(requestBuffer);
    uint32_t responseHeader = stream_recv_header(toSocket);
   // if (responseHeader != HEADER_read) 
    {
        log_error(cpuLogger, "Error al leer en memoria");
        exit(-1);
    }
    t_buffer *responseBuffer = buffer_create();
    stream_recv_buffer(toSocket, responseBuffer);
    uint32_t contenidoLeido = -1;
    buffer_unpack(responseBuffer, &contenidoLeido, sizeof(contenidoLeido));
    buffer_destroy(responseBuffer);
    return contenidoLeido;
}