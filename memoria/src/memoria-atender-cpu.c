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
            log_info(memoriaLogger, "\e[1;93mPetición de marco\e[0m");
            uint32_t base_segmento;
            uint32_t desplazamiento_segmento;
            uint32_t cantidadByte;
            
            buffer_unpack(buffer, &base_segmento, sizeof(base_segmento));
            buffer_unpack(buffer, &desplazamiento_segmento, sizeof(desplazamiento_segmento));
            buffer_unpack(buffer, &cantidadByte, sizeof(cantidadByte));

            pthread_mutex_lock(&mutexListaDeSegmento);
            Segmento* segementoSolic = obtener_segmento_por_BASE(base_segmento);
            pthread_mutex_unlock(&mutexListaDeSegmento);
            
            uint32_t retardoInstruccion = memoria_config_get_retardo_memoria(memoriaConfig);
            intervalo_de_pausa(retardoInstruccion);
            
            log_info(memoriaLogger, "Se quiere la dirección física del segmento con base <%i>", base_segmento);
            pid = segmento_get_pid(segementoSolic);

            t_buffer* respuestaBuffer = buffer_create();
            //Logica de desplazamiento ...          desplazamiento <= limite todo ok, si es > entonces hay segmentation fault
            if((desplazamiento_segmento + cantidadByte) > segmento_get_tamanio(segementoSolic)){
                uint32_t baseSegFault = -1;
                buffer_pack(respuestaBuffer, &baseSegFault, sizeof(baseSegFault));
                stream_send_buffer(socketCpu, HEADER_chequeo_DF,respuestaBuffer);   //seria el HEADER_SegFault

                printf("\nentra al if de segFautl\n");
                //log_info(memoriaLogger, "Segmentation Fault!! Fallo en el acceso al segmento <%i> con pid <%i>", id_segmento, pid);
                //Deberia finalizar el proceso
                // BORRAR TODA LAS TABLAS DE SEGMENTO DE ESE PROCESO 
                //--->>> lo hago cuando kernel me avisa que tengo que eliminar el proceso
            } else {
                buffer_pack(respuestaBuffer, &base_segmento, sizeof(base_segmento));
                stream_send_buffer(socketCpu, HEADER_chequeo_DF,respuestaBuffer);
                log_info(memoriaLogger, "Se enviá la base del segmento[%i]", segementoSolic->segmento_id);
            
            }
            
          
            buffer_destroy(respuestaBuffer);
        }
        break;
        
        case HEADER_move_in :{ //lee de memoria y lo guarda en el registro
            log_info(memoriaLogger, "\e[1;93mPetición de lectura\e[0m");
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
            
            /*char* contenidoAenviar = malloc(strlen(segmentoLeido->contenido) + 1);
            strcpy(contenidoAenviar, segmentoLeido->contenido);*/

            char* contenidoAenviarAux = malloc(cantidadByte + 1); // Buffer de destino para almacenar los datos leídos, se reserva un espacio adicional para el carácter nulo
            memset(contenidoAenviarAux, 0, cantidadByte + 1); // Inicializar el buffer con ceros
            memcpy(contenidoAenviarAux, memoriaPrincipal + (size_t)base_segmento +(size_t)desplazamiento_segmento, (size_t)cantidadByte);

            t_buffer* bufferContenido = buffer_create();        
        
            buffer_pack_string(bufferContenido, contenidoAenviarAux);

            stream_send_buffer(socketCpu, HEADER_move_in, bufferContenido);

            buffer_destroy(bufferContenido);    
            log_info(memoriaLogger, "Contenido leido : <%s> - En el segmento ID : <%i> ", contenidoAenviarAux, segmento_get_id(segmentoLeido));
            //buffer_destroy(buffer);
            //free(contenidoAenviarAux);
        }
        break;
        case HEADER_move_out : {//"escribir"
            log_info(memoriaLogger, "\e[1;93mPetición de escritura\e[0m");
            
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

            //cada vez que hago un obtener segmento hago un list_replace
            if (contenidoAEscribir != NULL) {
                memcpy(memoriaPrincipal + (size_t)base_segmento + (size_t)desplazamiento_segmento, contenidoAEscribir, (size_t)cantidadByte);
                //segmento_set_contenido(unSegmento, contenidoAEscribir);
            } else {
                memset(contenidoAEscribir, 0, (size_t)cantidadByte + 1); // Inicializar el buffer con ceros
            }
            /*pthread_mutex_lock(&mutexListaDeSegmento);
            modificarSegmento(base_segmento, unSegmento);    //es un obtener-segmento con list_replace
            pthread_mutex_unlock(&mutexListaDeSegmento);*/
            log_info(memoriaLogger, "Contenido escrito : <%s> - En el segmento ID : <%i> ", contenidoAEscribir, segmento_get_id(unSegmento));

            //buffer_destroy(buffer);
            break;
            
        }
        break;
        default:
        break;
    }
    }
}
