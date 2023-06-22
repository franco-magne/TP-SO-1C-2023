#include <../include/memoria-atender-cpu.h>
extern pthread_mutex_t mutexMemoriaData;
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
        case HEADER_marco :{
            //marco es la direccion Fisica
            log_info(memoriaLogger, "\e[1;93mPetición de marco\e[0m");
            double id_segmento;
            int pid;
            uint32_t desplazamiento_segmento;
            buffer_unpack(buffer, &id_segmento, sizeof(id_segmento));
            buffer_unpack(buffer, &pid, sizeof(pid));
            buffer_unpack(buffer, &desplazamiento_segmento, sizeof(desplazamiento_segmento));
            
            Segmento* segmentoSolic = obtener_segmento_por_id(pid,(int)id_segmento);
            log_info(memoriaLogger, "Se quiere la dirección física del segmento <%i>",id_segmento);
            
            //Logica de desplazamiento ...          base < limite
            /*if(desplazamiento_segmento >= segmento_get_limite(segementoSolic)){
                segmentation_fault...
            }*/
            
            uint32_t marco = desplazamiento_segmento + segmento_get_base(segmentoSolic);//obtener_marco(pid, id_segmento); //base y limite no +
            
            t_buffer* buffer_rta = buffer_create();
            buffer_pack(buffer_rta, &marco, sizeof(marco));
            stream_send_buffer(socketCpu, HEADER_marco, buffer_rta);
            buffer_destroy(buffer_rta);
            log_info(memoriaLogger, "Se enviá la dirección física [%d]", marco);
            
            //buffer_destroy(buffer);
        }
        break;
        case HEADER_move_in :{ //"leer"
            uint32_t pid;
            uint32_t id_segmento;
            uint32_t desplazamiento_segmento;

            buffer_unpack(buffer, &id_segmento, sizeof(id_segmento));
            buffer_unpack(buffer, &pid, sizeof(pid));
            buffer_unpack(buffer, &desplazamiento_segmento, sizeof(pid));

            Segmento* unSegmento = obtener_segmento_por_id(pid, id_segmento);
    
            char* contenidoAenviar = segmento_get_contenido(unSegmento);

            log_info(memoriaLogger, "Contenido leio : <%s> - En el segmento ID : <%i> ", contenidoAenviar, id_segmento);
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
