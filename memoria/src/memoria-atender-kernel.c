#include <../include/memoria-atender-kernel.h>


pthread_mutex_t mutexMemoriaData; //extern
pthread_mutex_t mutexTamanioActualMemoria = PTHREAD_MUTEX_INITIALIZER; //extern

extern t_log *memoriaLogger;
extern uint32_t tamActualMemoria;
extern tabla_de_segmentos* tabla_segmentos; //aca va con extern?

//t_config *memoriaConfig;

bool puedo_crear_segmento(uint32_t tamanio){
    return tamanio <= tamActualMemoria;  
}

void atender_peticiones_kernel(int socketKernel) {
    uint8_t header;
    for (;;) {
        header = stream_recv_header(socketKernel);
        pthread_mutex_lock(&mutexMemoriaData);
        t_buffer* buffer = buffer_create();
        stream_recv_buffer(socketKernel, buffer);

        switch (header) {
            case HEADER_create_segment: { //no seria HEADER_iniciarProceso  xq kernel no usa la tabla??
                log_info(memoriaLogger,"socket kernel -> 1- <%i> ", socketKernel);

                uint32_t tamanio_de_segmento;  
                uint32_t id_de_segmento;
                
                buffer_unpack(buffer, &id_de_segmento, sizeof(id_de_segmento));
                buffer_unpack(buffer, &tamanio_de_segmento, sizeof(tamanio_de_segmento));

                log_info(memoriaLogger,"tamanio de segmento <%i> y tamActualMemoria <%i>", tamanio_de_segmento, tamActualMemoria);

                if(!puedo_crear_segmento(tamanio_de_segmento)){
                    log_error(memoriaLogger, "No se puede crear el segmento, el tamanio supera espacio libre");
                    stream_send_empty_buffer(socketKernel, HANDSHAKE_seg_muy_grande);
                    break;
                }
                Segmento* unSegmento = crear_segmento(tamanio_de_segmento);
                segmento_set_id(unSegmento, id_de_segmento);
                segmento_set_socket(unSegmento, socketKernel);

                pthread_mutex_lock(&mutexTamanioActualMemoria);
                tamActualMemoria -= tamanio_de_segmento;
                estado_encolar_segmento_atomic(tabla_segmentos,unSegmento);
                pthread_mutex_unlock(&mutexTamanioActualMemoria);

                log_info(memoriaLogger, "\e[1;93mSe crea nuevo segmento con id [%i] y tamanio [%i]\e[0m", id_de_segmento, tamanio_de_segmento);

                stream_send_empty_buffer(socketKernel, HANDSHAKE_ok_continue);
                
                if( (list_size((estado_get_list(tabla_segmentos))) == 4)){
                for(int i = 0; i<(list_size((estado_get_list(tabla_segmentos)))); i++ ){
                    Segmento* x = estado_desencolar_primer_segmento_atomic(tabla_segmentos);
                    log_info(memoriaLogger, "id_segmento <%i> : ", x->segmento_id );
                }
                }
                //buffer_destroy(buffer);
                break;
            }
            /*case HEADER_delete_segment:{
                uint32_t id_segmento;
                buffer_unpack(buffer, &id_segmento, sizeof(id_segmento));

                log_info(memoriaLogger,"Segmento a eliminar <%i> ", id_segmento);

                if(!puedo_eliminar_segmento(id_segmento)){
                    log_error(memoriaLogger, "No se puede elimnar el segmento"); //agregar razon de por que no se pudo?
                    stream_send_empty_buffer(socketKernel, HANDSHAKE_seg_muy_grande);
                    break;                    
                }

                Segmento* unSegmento = obtener_segmento(id_segmento);




                break;
            }*/
            case HEADER_proceso_terminado:{
                 
            }
            default:
                //exit(-1);
                break;
        }
        pthread_mutex_unlock(&mutexMemoriaData);
    }
}


/*
                        t_buffer* buffer = buffer_create();

                
                //puedo obtener el tamanio de tabla de segmentos??? Si, sizeof(Segmento)*list_size(TdeSeg)
                buffer_unpack()
                buffer_unpack(buffer, &tamanioTdeSeg, sizeof(tamanio));
                if (puedo_crear_proceso(tamanioTdeSeg, memoriaData)) {
    
                    t_buffer* buffer_rta = buffer_create();
                    buffer_pack(buffer_rta, tablaDeSegmentos??, sizeof(tablaDeSegmentos)); DUDAS???

                    stream_send_buffer(socket, HANDSHAKE_ok_continue, buffer_rta);
                    buffer_destroy(buffer_rta);Segmento *segCompartido = malloc(sizeof(*segCompartido));
                    log_info(memoriaLogger, "Se asigno correctamente una tabla de segmentos con tamaño [%d]", tamanioTdeSeg);
                } else {
                    stream_send_empty_buffer(socket, HEADER_error);
                    log_error(memoriaLogger, "No se pudo asignar tabla de segmentos con tamaño [%d]", tamanioTdeSeg);
                }
                buffer_destroy(buffer);*/