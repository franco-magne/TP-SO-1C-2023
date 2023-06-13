#include <../include/memoria-atender-kernel.h>


pthread_mutex_t mutexMemoriaData; //extern
pthread_mutex_t mutexTamanioActualMemoria = PTHREAD_MUTEX_INITIALIZER; //extern

extern t_log *memoriaLogger;
extern t_memoria_config* memoriaConfig;
extern uint32_t tamActualMemoria;
extern t_estado* tabla_segmentos; 
extern Segmento* segCompartido;

extern t_list* listaDeProcesos;

//t_config *memoriaConfig;

bool puedo_crear_segmento_o_proceso(uint32_t tamanio){
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
            case HEADER_iniciar_proceso:{
                int pid;
                t_list* tabla_segmentos_solicitada = list_create();
                buffer_unpack(buffer, &pid, sizeof(pid));

                if(!puedo_crear_segmento_o_proceso(memoria_config_get_tamanio_segmento_0(memoriaConfig))){ //+sizeof(*tabla_de_segmentos)
                    log_error(memoriaLogger, "No se pudo crear el proceso, no hay espacio en memoria");
                }
                inicializar_estructuras();
                Procesos* proceso = crear_proceso(); //proceso inicilizado con SegCompartido en la tabla de segmentos
                list_add(listaDeProcesos, proceso);
                tabla_segmentos_solicitada = proceso->tablaDeSegmentos;
                //aca dentro deberia restar el tamActualMemoria
                
                t_buffer* buffer_rta = buffer_create();
                buffer_pack(buffer_rta, tabla_segmentos_solicitada, sizeof(tabla_segmentos_solicitada));
                stream_send_empty_buffer(socketKernel, HANDSHAKE_ok_continue);
                stream_send_buffer(socketKernel, HEADER_tabla_segmentos, buffer_rta);

                //buffer_destroy(buffer);
            }
            case HEADER_create_segment: { 
                log_info(memoriaLogger,"socket kernel -> 1- <%i> ", socketKernel);

                uint32_t tamanio_de_segmento;  
                uint32_t id_de_segmento;
                
                buffer_unpack(buffer, &id_de_segmento, sizeof(id_de_segmento));
                buffer_unpack(buffer, &tamanio_de_segmento, sizeof(tamanio_de_segmento));

                log_info(memoriaLogger,"tamanio de segmento <%i> y tamActualMemoria <%i>", tamanio_de_segmento, tamActualMemoria);

                // (!) este if es clave, borrar todos los segments del Proceso que 
                // se crearon antes si es que uno de sus segments no se puede crear.
                if(!puedo_crear_segmento_o_proceso(tamanio_de_segmento)){//si no puedo crear otro segmento de ese proceso tengo que avisar a kernel y hacer un deleteSegment del resto
                    log_error(memoriaLogger, "No se puede crear el segmento, el tamanio supera espacio libre");

                    stream_send_empty_buffer(socketKernel, HANDSHAKE_seg_muy_grande); //(!) hay que avisarle a kernel que no se puede
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
                    //por que no se podria eliminar un segmento?
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