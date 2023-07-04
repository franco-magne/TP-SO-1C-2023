#include <../include/memoria-atender-FS.h>

pthread_mutex_t mutexMemoriaData = PTHREAD_MUTEX_INITIALIZER; //extern
extern t_log *memoriaLogger;

void atender_peticiones_fileSystem(int socketFS) {
    uint8_t header;
    for (;;) {
        header = stream_recv_header(socketFS);
        pthread_mutex_lock(&mutexMemoriaData);
        t_buffer* buffer = buffer_create();
        stream_recv_buffer(socketFS, buffer);

        switch (header) {
            case HEADER_f_read:{
                log_info(memoriaLogger, "\e[1;93mPetición de escritura\e[0m");
            
                uint32_t pid;
                uint32_t id_segmento;            
                char* contenidoAEscribir;
                buffer_unpack(buffer, &id_segmento, sizeof(id_segmento));
                buffer_unpack(buffer, &pid, sizeof(pid));
                contenidoAEscribir = buffer_unpack_string(buffer);
                
                Segmento* unSegmento = obtener_segmento_por_id(pid, id_segmento);
                //cada vez que hago un obtener segmento hago un list_replace
                segmento_set_contenido(unSegmento, contenidoAEscribir);

                log_info(memoriaLogger, "Contenido escrito : <%s> - En el segmento ID : <%i> ", contenidoAEscribir, id_segmento);
                break;
            }
            case HEADER_f_write:{
                uint32_t pid;
                uint32_t id_segmento;
                //uint32_t desplazamiento_segmento;

                buffer_unpack(buffer, &id_segmento, sizeof(id_segmento));
                buffer_unpack(buffer, &pid, sizeof(pid));
                //buffer_unpack(buffer, &desplazamiento_segmento, sizeof(pid));

                //Segmento* unSegmento = obtener_segmento_por_id(pid, id_segmento);
        
                char* contenidoAenviar = malloc(strlen("HOLA") + 1);  // Reservas memoria suficiente para la cadena "HOLA" y el carácter nulo
                strcpy(contenidoAenviar, "HOLA");
                t_buffer* bufferContenido = buffer_create();        
            
                buffer_pack_string(bufferContenido, contenidoAenviar);

                stream_send_buffer(socketFS, HEADER_move_in, bufferContenido);

                buffer_destroy(bufferContenido);
                break;
            }
            default:
                break;
        }
    }
}