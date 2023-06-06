#include <../include/memoria-atender-cpu.h>

extern pthread_mutex_t mutexMemoriaData;
extern t_memoria_config *memoriaConfig;
extern t_log *memoriaLogger;

void* escuchar_peticiones_cpu(void* socketCpu) {
    void* memoriaPrincipal = memoriaPrincipal;
    int socket = *(int*)socketCpu;
    uint32_t header, direccionFisica, valor;
    t_buffer* buffer;
    for (;;) {
        header = stream_recv_header(socket);
        pthread_mutex_lock(&mutexMemoriaData);
        buffer = buffer_create();
        stream_recv_buffer(socket, buffer);
        intervalo_de_pausa(memoria_config_get_retardo_memoria(memoriaConfig));

        switch (header) {
            case HEADER_read:
                log_info(memoriaLogger, "\e[1;93mPetición de lectura\e[0m");

                // Desempaqueta buffer para obtener la -direccion fisica- que se solicito
                buffer_unpack(buffer, &direccionFisica, sizeof(direccionFisica));
                
                // Copia el valor de la direccion fisica solicitada a la variable "valor"
                memcpy((void*)&valor, (void*)(memoriaPrincipal + direccionFisica), sizeof(valor));

                // Se crea un nuevo buffer para enviar la respuesta (Respuesta a la peticion LECTURA)
                t_buffer* buffer_rta = buffer_create();
                buffer_pack(buffer_rta, &valor, sizeof(valor));
                stream_send_buffer(socket, HEADER_read, buffer_rta);
                buffer_destroy(buffer_rta);

                //__actualizar_pagina(direccionFisica, false, memoriaData); // No se utiliza paginacion
                log_info(memoriaLogger, "Se envió el valor [%d] de la dirección física [%d]", valor, direccionFisica);
                break;
            case HEADER_write:
                log_info(memoriaLogger, "\e[1;93mPetición de escritura\e[0m");

                // Desempaqueta buffer para obtener -direccion fisica- y -valor- a escribir
                buffer_unpack(buffer, &direccionFisica, sizeof(direccionFisica));
                buffer_unpack(buffer, &valor, sizeof(valor));

                // Copia el valor a escribir en la -dirección física- especificada
                memcpy((void*)(memoriaPrincipal + direccionFisica), (void*)&valor, sizeof(valor));

                //__actualizar_pagina(direccionFisica, true, memoriaData); // No se utiliza paginacion
                log_info(memoriaLogger, "Se escribio el valor [%d] en la dirección física [%d]", *((uint32_t*)(memoriaPrincipal + direccionFisica)), direccionFisica);
                break;
            default:
                break;
        }
        

        buffer_destroy(buffer);
        pthread_mutex_unlock(&mutexMemoriaData);

    }
}
