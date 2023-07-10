#include <../include/memoria-atender-FS.h>

extern pthread_mutex_t mutexMemoriaData; //extern
extern pthread_mutex_t mutexListaDeSegmento;
extern t_log *memoriaLogger;
extern t_list* listaDeSegmentos;

void atender_peticiones_fileSystem(int socketFS) {
    uint8_t header;
    for (;;) {
        header = stream_recv_header(socketFS);
        t_buffer* buffer = buffer_create();
        stream_recv_buffer(socketFS, buffer);

        switch (header) {
            case HEADER_f_read:{        //Lee del archivo y escribe en memoria
                uint32_t base_segmento;
                uint32_t desplazamiento_segmento;
                
                buffer_unpack(buffer, &base_segmento, sizeof(base_segmento));
                char* contenidoAEscribir;
                contenidoAEscribir = buffer_unpack_string(buffer);
                
                Segmento* unSegmento = obtener_segmento_por_BASE(base_segmento);
                //cada vez que hago un obtener segmento hago un list_replace
                segmento_set_contenido(unSegmento, contenidoAEscribir);
                pthread_mutex_lock(&mutexListaDeSegmento);
                modificarSegmento(base_segmento, unSegmento);    //es un obtener-segmento con list_replace
                pthread_mutex_unlock(&mutexListaDeSegmento);
                log_info(memoriaLogger, "Contenido escrito : <%s> - En el segmento ID : <%i> ", contenidoAEscribir, segmento_get_id(unSegmento));
                stream_send_empty_buffer(socketFS, HANDSHAKE_ok_continue);

                break;
            }
            case HEADER_f_write:{
                uint32_t base_segmento;
              //  uint32_t desplazamiento_segmento;
                buffer_unpack(buffer, &base_segmento, sizeof(base_segmento));
              //  buffer_unpack(buffer, &desplazamiento_segmento, sizeof(desplazamiento_segmento));
                char* contenidoAenviar;
                Segmento* segmentoLeido = obtener_segmento_por_BASE(base_segmento);
              if (segmentoLeido->contenido != NULL) {
                contenidoAenviar = malloc(strlen(segmentoLeido->contenido) + 1);
                strcpy(contenidoAenviar, segmentoLeido->contenido);
                } else {
                contenidoAenviar = malloc(strlen(" ") + 1);
                strcpy(contenidoAenviar, " ");
                }
                log_info(memoriaLogger,BOLD  BLUE UNDERLINE "Escritura en el segmento <%i> - Enviamos "RESET BOLD GREEN" <%s> ", base_segmento, contenidoAenviar) ;

                t_buffer* bufferContenido = buffer_create();        
            
                buffer_pack_string(bufferContenido, contenidoAenviar);

                stream_send_buffer(socketFS, HEADER_f_write, bufferContenido);

                buffer_destroy(bufferContenido);  
                    break;
            }
            default:
                break;
        }
    }
}