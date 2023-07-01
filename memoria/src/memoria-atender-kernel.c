#include <../include/memoria-atender-kernel.h>


pthread_mutex_t mutexMemoriaData = PTHREAD_MUTEX_INITIALIZER; //extern
pthread_mutex_t mutexTamMemoriaActual = PTHREAD_MUTEX_INITIALIZER; //extern
pthread_mutex_t mutexListaDeSegmento = PTHREAD_MUTEX_INITIALIZER;
extern t_log *memoriaLogger;
extern t_memoria_config* memoriaConfig;
extern uint32_t tamActualMemoria;
extern Segmento* segCompartido;


extern t_list* listaDeSegmentos; //la VERDADERA

//t_config *memoriaConfig;

bool puedo_crear_proceso_o_segmento(uint32_t tamanio){ //cambiar, solo puedo crear si tengo algun hueco libre de tamanio > tamanio
    return tamanio <= 4096;  
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
                buffer_unpack(buffer, &pid, sizeof(pid));
                //t_list* tabla_segmentos_solicitada = obtener_tabla_de_segmentos_por_pid(pid);

                if(!puedo_crear_proceso_o_segmento(memoria_config_get_tamanio_segmento_0(memoriaConfig))){ //+sizeof(*tabla_de_segmentos)
                    log_error(memoriaLogger, "No se pudo crear el proceso, no hay espacio en memoria");
                    stream_send_empty_buffer(socketKernel, HEADER_memoria_insuficiente);
                  
                } else {
                
                t_buffer* buffer_rta = buffer_create();
                t_segmento* segmentoCero = segmento_kernel_create();
                t_list* tabla_segmentos_solicitada = list_create();
                list_add(tabla_segmentos_solicitada,segmentoCero);
                buffer_pack_segmento_list(buffer_rta,tabla_segmentos_solicitada);
                stream_send_buffer(socketKernel, HEADER_tabla_segmentos, buffer_rta);
                log_info(memoriaLogger, "SEGMENTO 0 CREADO PARA PROCESO PID : <%i> ", pid);
                //list_destroy(tabla_segmentos_solicitada);
                buffer_destroy(buffer_rta);
                
             
                break;
                }
            
             break;
            }
            case HEADER_create_segment: { 
               // log_info(memoriaLogger,"socket kernel -> 1- <%i> ", socketKernel);

                uint32_t tamanio_de_segmento;  
                uint32_t id_de_segmento;
                int pid;
                buffer_unpack(buffer, &id_de_segmento, sizeof(id_de_segmento));
                buffer_unpack(buffer, &tamanio_de_segmento, sizeof(tamanio_de_segmento));
                buffer_unpack(buffer, &pid, sizeof(pid));

                log_info(memoriaLogger,"tamanio de segmento <%i> con id <%i>", tamanio_de_segmento, id_de_segmento);

                // (!) este if es clave, borrar todos los segments del Proceso que 
                // se crearon antes si es que uno de sus segments no se puede crear.
                if(!puedo_crear_proceso_o_segmento(tamanio_de_segmento)){//si no puedo crear otro segmento de ese proceso tengo que avisar a kernel y hacer un deleteSegment del resto

                    liberar_tabla_segmentos(pid);
                    log_error(memoriaLogger, "No se puede crear el segmento, el tamanio supera espacio libre, liberamos la tabla de segmentos");

                    stream_send_empty_buffer(socketKernel, HANDSHAKE_seg_muy_grande); //(!) hay que avisarle a kernel que no se puede
                    break;
                }

                Segmento* unSegmento = crear_segmento(tamanio_de_segmento);
                segmento_set_id(unSegmento, id_de_segmento);
                segmento_set_pid(unSegmento, pid);
                log_info(memoriaLogger, "\e[1;93mSe crea nuevo segmento con id [%i] y tamanio [%i]\e[0m", id_de_segmento, tamanio_de_segmento);
                pthread_mutex_lock(&mutexTamMemoriaActual);
                //tamActualMemoria -= tamanio_de_segmento;
                administrar_nuevo_segmento(unSegmento);
                pthread_mutex_unlock(&mutexTamMemoriaActual);

                pthread_mutex_lock(&mutexListaDeSegmento);
                t_list* tablaDeSegmentoActualizada = listaDeSegmentos;
                tablaDeSegmentoActualizada = list_filter_ok(tablaDeSegmentoActualizada,segmentos_del_mismo_pid, unSegmento);
                tablaDeSegmentoActualizada = list_map(tablaDeSegmentoActualizada,adapter_segmento_memoria_kernel);
                pthread_mutex_unlock(&mutexListaDeSegmento);

                t_buffer* bufferTablaDeSegmentoActualizada = buffer_create();
                buffer_pack_segmento_list(bufferTablaDeSegmentoActualizada, tablaDeSegmentoActualizada);
                stream_send_buffer(socketKernel, HANDSHAKE_ok_continue,bufferTablaDeSegmentoActualizada);
                buffer_destroy(bufferTablaDeSegmentoActualizada);
               
                //log_info(memoriaLogger, "\e[1;93mSe crea nuevo segmento con id [%i] y tamanio [%i]\e[0m", id_de_segmento, tamanio_de_segmento);
                
                t_list* tablaDeSegmentoSolic = obtener_tabla_de_segmentos_por_pid(pid);

                mostrar_lista_segmentos(listaDeSegmentos);
                break;
            }
            case HEADER_delete_segment:{
                //en caso de que tenga huecos libres aledaños, los deberá CONSOLIDAR actualizando sus estructuras administrativas.
                uint32_t id_de_segmento;
                int pid;
                buffer_unpack(buffer, &id_de_segmento, sizeof(id_de_segmento));
                buffer_unpack(buffer, &pid, sizeof(pid));

                t_list* tabla_segmentos_solic = obtener_tabla_de_segmentos_por_pid(pid);
                log_info(memoriaLogger,"Tabla de Segmentos con PID<%d> ", pid);
                

                log_info(memoriaLogger,"Segmento a eliminar <%i> ", id_de_segmento);

                Segmento* segABorrar = crear_segmento(-1);
                segmento_set_id(segABorrar, id_de_segmento);
                segmento_set_pid(segABorrar, pid);
                //sumar_memoriaRecuperada_a_tamMemoriaActual(segABorrar->tamanio); 
                eliminar_segmento_memoria(segABorrar);
                log_info(memoriaLogger, "Borramos el segmento <%i> del proceos <%i>", id_de_segmento, pid);

                t_list* tabla_segmentos_solic_actualizada = obtener_tabla_de_segmentos_por_pid(pid);
                log_info(memoriaLogger,"Tabla de Segmentos con PID <%d> actualizada ", pid);
                mostrar_lista_segmentos(listaDeSegmentos);


                stream_send_empty_buffer(socketKernel, HANDSHAKE_ok_continue);

                break;
            }
            /*case HEADER_proceso_terminado: {
                int pid;
                buffer_unpack(buffer, &pid, sizeof(pid));
                liberar_tabla_segmentos(pid);
                stream_send_empty_buffer(socketKernel, HEADER_proceso_terminado);
            }*/
            default:
                //exit(-1);
                break;
        }
        pthread_mutex_unlock(&mutexMemoriaData);
        buffer_destroy(buffer);
    }
}

