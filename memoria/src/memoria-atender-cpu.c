#include <../include/memoria-atender-cpu.h>

extern pthread_mutex_t mutexMemoriaData;
extern pthread_mutex_t mutexListaDeSegmento;
extern t_log *memoriaLogger;
extern t_memoria_config* memoriaConfig;
extern uint32_t tamActualMemoria;
extern void* memoriaPrincipal;
extern Segmento* segCompartido;

void atender_peticiones_cpu(int socketCpu) {
    uint8_t header;
     while ((header = stream_recv_header(socketCpu)) != -1) {
        t_buffer* buffer = buffer_create();
        stream_recv_buffer(socketCpu, buffer);
        switch (header){

        case HEADER_chequeo_DF :{ //acceso a memoria  HEADER_chequeo_DF
            //marco es la direccion Fisica
            uint32_t pid;
            uint32_t base_segmento;
            uint32_t desplazamiento_segmento;
            uint32_t cantidadByte;
            
            buffer_unpack(buffer, &base_segmento, sizeof(base_segmento));
            buffer_unpack(buffer, &desplazamiento_segmento, sizeof(desplazamiento_segmento));
            buffer_unpack(buffer, &cantidadByte, sizeof(cantidadByte));

            pthread_mutex_lock(&mutexListaDeSegmento);
            Segmento* segementoSolic = obtener_segmento_por_BASE(base_segmento);
            pthread_mutex_unlock(&mutexListaDeSegmento);
          
            t_buffer* respuestaBuffer = buffer_create();

           
                buffer_pack(respuestaBuffer, &segementoSolic->tamanio, sizeof(segementoSolic->tamanio));
                stream_send_buffer(socketCpu, HEADER_chequeo_DF,respuestaBuffer);
            
            
            buffer_destroy(respuestaBuffer);
        }
        break;
        
        case HEADER_move_in :{ //lee de memoria y lo guarda en el registro
            log_info(memoriaLogger, "\e[1;93mPETICION DE LECTURA CPU\e[0m");
            uint32_t base_segmento;
            uint32_t desplazamiento_segmento;
            uint32_t cantidadByte;
            
            buffer_unpack(buffer, &base_segmento, sizeof(base_segmento));
            buffer_unpack(buffer, &desplazamiento_segmento, sizeof(desplazamiento_segmento));
            buffer_unpack(buffer, &cantidadByte, sizeof(cantidadByte));
            
            pthread_mutex_lock(&mutexListaDeSegmento);
            Segmento* segmentoLeido = obtener_segmento_por_BASE(base_segmento);
            //aca acceso al void* y leo 

            pthread_mutex_unlock(&mutexListaDeSegmento);

            uint32_t retardoInstruccion = memoria_config_get_retardo_memoria(memoriaConfig);
            intervalo_de_pausa(retardoInstruccion);
            
    

            char* contenidoAenviarAux = malloc(cantidadByte + 1); // Buffer de destino para almacenar los datos leídos, se reserva un espacio adicional para el carácter nulo
            memset(contenidoAenviarAux, 0, cantidadByte + 1); // Inicializar el buffer con ceros
            memcpy(contenidoAenviarAux, memoriaPrincipal + (size_t)base_segmento +(size_t)desplazamiento_segmento, (size_t)cantidadByte);

            t_buffer* bufferContenido = buffer_create();        
        
            buffer_pack_string(bufferContenido, contenidoAenviarAux);

            stream_send_buffer(socketCpu, HEADER_move_in, bufferContenido);

            buffer_destroy(bufferContenido);    
            log_info(memoriaLogger,BLUE ITALIC " CONTENIDO LEIDO : "RESET YELLOW "<%s> "RESET BLUE ITALIC" - EN EL SEGMENTO ID : <%i> ", contenidoAenviarAux, segmento_get_id(segmentoLeido));
            log_info(memoriaLogger,BOLDMAGENTA " PID: "RESET BOLDGREEN"<%i>"RESET BOLDMAGENTA" - Acción: "RESET BOLDGREEN"<LEER>"RESET BOLDMAGENTA" - Dirección física: "RESET BOLDGREEN"<(%i|%i)>"RESET BOLDMAGENTA" - Tamaño: "RESET BOLDGREEN"<%i>"RESET BOLDMAGENTA" - Origen: "RESET BOLDGREEN"<CPU> ",segmento_get_pid(segmentoLeido),base_segmento,desplazamiento_segmento, segmento_get_tamanio(segmentoLeido));

            //buffer_destroy(buffer);
            //free(contenidoAenviarAux);
        }
        break;
        case HEADER_move_out : {//"escribir"
            log_info(memoriaLogger, "\e[1;93mPetición de escritura CPU\e[0m");
            
            uint32_t base_segmento;
            uint32_t desplazamiento_segmento;
            uint32_t cantidadByte;
            
            buffer_unpack(buffer, &base_segmento, sizeof(base_segmento));
            buffer_unpack(buffer, &desplazamiento_segmento, sizeof(desplazamiento_segmento));
            buffer_unpack(buffer, &cantidadByte, sizeof(cantidadByte));

            char* contenidoAEscribir;
            contenidoAEscribir = buffer_unpack_string(buffer);
            
            pthread_mutex_lock(&mutexListaDeSegmento);
            Segmento* unSegmento = obtener_segmento_por_BASE(base_segmento);
            pthread_mutex_unlock(&mutexListaDeSegmento);

            uint32_t retardoInstruccion = memoria_config_get_retardo_memoria(memoriaConfig);
            intervalo_de_pausa(retardoInstruccion);

            if (contenidoAEscribir != NULL) {
                memcpy(memoriaPrincipal + (size_t)base_segmento + (size_t)desplazamiento_segmento, contenidoAEscribir, (size_t)cantidadByte);
            } else {
                memset(contenidoAEscribir, 0, (size_t)cantidadByte + 1); // Inicializar el buffer con ceros
            }
           
            log_info(memoriaLogger, "CONTENIDO LEIDO : "RESET YELLOW "<%s> "RESET BLUE ITALIC " - EN EL SEGMENTO ID : <%i>" , contenidoAEscribir, segmento_get_id(unSegmento));
            log_info(memoriaLogger,BOLDMAGENTA " PID: "RESET BOLDGREEN"<%i>"RESET BOLDMAGENTA" - Acción: "RESET BOLDGREEN"<LEER>"RESET BOLDMAGENTA" - Dirección física: "RESET BOLDGREEN"<(%i|%i)>"RESET BOLDMAGENTA" - Tamaño: "RESET BOLDGREEN"<%i>"RESET BOLDMAGENTA" - Origen: "RESET BOLDGREEN"<CPU> ",segmento_get_pid(unSegmento),base_segmento,desplazamiento_segmento, segmento_get_tamanio(unSegmento));
            //buffer_destroy(buffer);
            break;
            
        }
        break;
        default:
        break;
    }
    }
}
