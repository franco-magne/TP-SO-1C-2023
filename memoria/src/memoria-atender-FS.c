#include <../include/memoria-atender-FS.h>

extern pthread_mutex_t mutexMemoriaData; //extern
extern pthread_mutex_t mutexListaDeSegmento;
extern t_log *memoriaLogger;
extern t_list* listaDeSegmentos;
extern void* memoriaPrincipal;

void atender_peticiones_fileSystem(int socketFS) {
    uint8_t header;
    for (;;) {
        header = stream_recv_header(socketFS);
        t_buffer* buffer = buffer_create();
        stream_recv_buffer(socketFS, buffer);

        switch (header) {
            case HEADER_f_read:{        //Lee del archivo y escribe en memoria
                log_info(memoriaLogger, "\e[1;93mPETICION DE ESCRITURA FS\e[0m");
                uint32_t base_segmento;
                uint32_t desplazamiento_segmento;
                uint32_t cantidadByte;
                
                buffer_unpack(buffer, &base_segmento, sizeof(base_segmento));
                buffer_unpack(buffer, &desplazamiento_segmento, sizeof(desplazamiento_segmento));
                buffer_unpack(buffer, &cantidadByte, sizeof(cantidadByte));

                char* contenidoAEscribir = malloc(cantidadByte + 1);
                contenidoAEscribir = buffer_unpack_string(buffer);
                
                Segmento* unSegmento = obtener_segmento_por_BASE(base_segmento);
                //cada vez que hago un obtener segmento hago un list_replace
                segmento_set_contenido(unSegmento, contenidoAEscribir);
                pthread_mutex_lock(&mutexListaDeSegmento);
                modificarSegmento(base_segmento, unSegmento);    //es un obtener-segmento con list_replace
                pthread_mutex_unlock(&mutexListaDeSegmento);

                  if (contenidoAEscribir != NULL) {
                memcpy(memoriaPrincipal + (size_t)base_segmento + (size_t)desplazamiento_segmento, contenidoAEscribir, (size_t)cantidadByte);
                //segmento_set_contenido(unSegmento, contenidoAEscribir);
                } else {
                memset(contenidoAEscribir, 0, (size_t)cantidadByte + 1); // Inicializar el buffer con ceros
                }
                log_info(memoriaLogger, "Contenido escrito : <%s> - En el segmento ID : <%i> ", contenidoAEscribir, segmento_get_id(unSegmento));
                log_info(memoriaLogger,BOLDMAGENTA " PID: "RESET BOLDGREEN"<%i>"RESET BOLDMAGENTA" - Acción: "RESET BOLDGREEN"<ESCRIBIR>"RESET BOLDMAGENTA" - Dirección física: "RESET BOLDGREEN"<(%i|%i)>"RESET BOLDMAGENTA" - Tamaño: "RESET BOLDGREEN"<%i>"RESET BOLDMAGENTA" - Origen: "RESET BOLDGREEN"<CPU> ",segmento_get_pid(unSegmento),base_segmento,desplazamiento_segmento, segmento_get_tamanio(unSegmento));

                stream_send_empty_buffer(socketFS, HANDSHAKE_ok_continue);

                free(contenidoAEscribir);
                break;
            }
            case HEADER_f_write:{   //Lee de memoria y lo escribe en el archivo
                log_info(memoriaLogger, "\e[1;93mPETICION DE LECTURA FS\e[0m");
                uint32_t base_segmento;
                uint32_t desplazamiento_segmento;
                uint32_t cantidadByte;
                
                buffer_unpack(buffer, &base_segmento, sizeof(base_segmento));
                buffer_unpack(buffer, &desplazamiento_segmento, sizeof(desplazamiento_segmento));
                buffer_unpack(buffer, &cantidadByte, sizeof(cantidadByte));
                

                Segmento* segmentoLeido = obtener_segmento_por_BASE(base_segmento);

                char* contenidoAenviar = malloc(cantidadByte + 1); // Buffer de destino para almacenar los datos leídos, se reserva un espacio adicional para el carácter nulo
                memset(contenidoAenviar, 0, cantidadByte + 1); // Inicializar el buffer con ceros
                memcpy(contenidoAenviar, memoriaPrincipal + (size_t)base_segmento +(size_t)desplazamiento_segmento, (size_t)cantidadByte);
                log_info(memoriaLogger, "Contenido lectura : <%s> - En el segmento ID : <%i> ", contenidoAenviar, segmento_get_id(segmentoLeido));
                log_info(memoriaLogger,BOLDMAGENTA " PID: "RESET BOLDGREEN"<%i>"RESET BOLDMAGENTA" - Acción: "RESET BOLDGREEN"<LEER>"RESET BOLDMAGENTA" - Dirección física: "RESET BOLDGREEN"<(%i|%i)>"RESET BOLDMAGENTA" - Tamaño: "RESET BOLDGREEN"<%i>"RESET BOLDMAGENTA" - Origen: "RESET BOLDGREEN"<FS> ",segmento_get_pid(segmentoLeido),base_segmento,desplazamiento_segmento, segmento_get_tamanio(segmentoLeido));

                t_buffer* bufferContenido = buffer_create();        
            
                buffer_pack_string(bufferContenido, contenidoAenviar);

                stream_send_buffer(socketFS, HEADER_f_write, bufferContenido);

                free(contenidoAenviar);
                buffer_destroy(bufferContenido);  
                break;
            }
            default:
                break;
        }
        buffer_destroy(buffer);
    }
}