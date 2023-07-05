#include <../include/memoria-atender-cpu.h>
extern pthread_mutex_t mutexMemoriaData;
extern pthread_mutex_t mutexListaDeSegmento;
extern t_log *memoriaLogger;
extern t_memoria_config* memoriaConfig;
extern uint32_t tamActualMemoria;
extern Segmento* segCompartido;


void atender_peticiones_cpu(int socketCpu) {
    uint8_t header;
     while ((header = stream_recv_header(socketCpu)) != -1) {
        t_buffer* buffer = buffer_create();
        stream_recv_buffer(socketCpu, buffer);
        switch (header){

        case HEADER_marco :{ //acceso a memoria  HEADER_chequeo_DF
            //marco es la direccion Fisica
            log_info(memoriaLogger, "\e[1;93mPetición de marco\e[0m");
            uint32_t id_segmento;
            int pid;
            uint32_t desplazamiento_segmento;
            buffer_unpack(buffer, &id_segmento, sizeof(id_segmento));
            buffer_unpack(buffer, &pid, sizeof(pid));
            buffer_unpack(buffer, &desplazamiento_segmento, sizeof(desplazamiento_segmento));
            
            Segmento* segementoSolic = obtener_segmento_por_id(pid, id_segmento);
            log_info(memoriaLogger, "Se quiere la dirección física del segmento <%i>",id_segmento);
            
            //Logica de desplazamiento ...          desplazamiento <= limite todo ok, si es > entonces hay segmentation fault
            if(desplazamiento_segmento > segmento_get_tamanio(segementoSolic)){
                stream_send_empty_buffer(socketCpu, HEADER_Segmentation_fault);
                log_error(memoriaLogger, "Segmentation Fault!! Fallo en el acceso al segmento <%i> con pid <%i>", id_segmento, pid);
                //Deberia finalizar el proceso
                break;
            }
            
            uint32_t marco = segmento_get_id(segementoSolic);//obtener_marco(pid, id_segmento); //base y limite no +
            
            t_buffer* buffer_rta = buffer_create();
            buffer_pack(buffer_rta, &marco, sizeof(marco));
            stream_send_buffer(socketCpu, HEADER_marco, buffer_rta);
            buffer_destroy(buffer_rta);
            log_info(memoriaLogger, "Se enviá la dirección física [%d]", marco);
            
            //buffer_destroy(buffer);
        }
        break;
        case HEADER_move_in :{ //lee de memoria y lo guarda en el registro
            uint32_t pid;
            uint32_t id_segmento;
            //uint32_t desplazamiento_segmento;

            buffer_unpack(buffer, &id_segmento, sizeof(id_segmento));
            buffer_unpack(buffer, &pid, sizeof(pid));
            //buffer_unpack(buffer, &desplazamiento_segmento, sizeof(pid));

            Segmento* segmentoLeido = obtener_segmento_por_id(pid, id_segmento);
            char* contenidoAenviar = malloc(strlen(segmentoLeido->contenido) + 1);
            strcpy(contenidoAenviar, segmentoLeido->contenido);

            t_buffer* bufferContenido = buffer_create();        
        
            buffer_pack_string(bufferContenido, contenidoAenviar);

            stream_send_buffer(socketCpu, HEADER_move_in, bufferContenido);

            buffer_destroy(bufferContenido);    

            //buffer_destroy(buffer);
        }
        break;
        case HEADER_move_out : {//"escribir"
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
            pthread_mutex_lock(&mutexListaDeSegmento);
            modificarSegmento(pid, id_segmento, unSegmento);    //es un obtener-segmento con list_replace
            pthread_mutex_unlock(&mutexListaDeSegmento);
            log_info(memoriaLogger, "Contenido escrito : <%s> - En el segmento ID : <%i> ", contenidoAEscribir, id_segmento);

            //buffer_destroy(buffer);
            break;
            
        }
        break;
        default:
        break;
    }
    }
}
