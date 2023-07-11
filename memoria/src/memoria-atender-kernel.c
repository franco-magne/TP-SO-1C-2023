#include <../include/memoria-atender-kernel.h>


extern pthread_mutex_t mutexMemoriaData; 
extern pthread_mutex_t mutexListaDeSegmento;
extern t_log *memoriaLogger;
extern t_memoria_config* memoriaConfig;
extern Segmento* segCompartido;


extern t_list* listaDeSegmentos; //la VERDADERA

//t_config *memoriaConfig;



void atender_peticiones_kernel(int socketKernel) {
    uint8_t header;
    for (;;) {
        header = stream_recv_header(socketKernel);
        pthread_mutex_lock(&mutexMemoriaData);
        t_buffer* buffer = buffer_create();
        stream_recv_buffer(socketKernel, buffer);

        switch (header) {
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

                    stream_send_empty_buffer(socketKernel, HEADER_memoria_insuficiente); //(!) hay que avisarle a kernel que no se puede
                    break;
                }
                
                restar_a_tamMemoriaActual(tamanio_de_segmento);

                Segmento* unSegmento = crear_segmento(tamanio_de_segmento);
                segmento_set_id(unSegmento, id_de_segmento);
                segmento_set_pid(unSegmento, pid);
                log_info(memoriaLogger, "\e[1;93mSe crea nuevo segmento de pid [%i] con id [%i] y tamanio [%i]\e[0m",pid , id_de_segmento, tamanio_de_segmento);
                
                uint32_t retardoInstruccion = memoria_config_get_retardo_memoria(memoriaConfig);
                intervalo_de_pausa(retardoInstruccion);

                pthread_mutex_lock(&mutexListaDeSegmento);
                uint8_t memoriaRespuesta = administrar_nuevo_segmento(unSegmento);
                pthread_mutex_unlock(&mutexListaDeSegmento);

                if(memoriaRespuesta == HEADER_Compactacion){
                    stream_send_empty_buffer(socketKernel,HEADER_Compactacion);
                    uint8_t respuestaDeKernel = stream_recv_header(socketKernel);
                    stream_recv_empty_buffer(socketKernel);
                    if(respuestaDeKernel == HANDSHAKE_ok_continue){
                        iniciar_compactacion();
                        log_info(memoriaLogger, MAGENTA ITALIC "SE INICIA LA COMPACTACION DE LA MEMORIA");
                        stream_send_empty_buffer(socketKernel, HEADER_Compactacion_finalizada);
                    }
                } 
                else {
                    pthread_mutex_lock(&mutexListaDeSegmento);
                    t_list* tablaDeSegmentoActualizada = listaDeSegmentos;
                    tablaDeSegmentoActualizada = list_filter(tablaDeSegmentoActualizada,segmentos_validez_1);
                    tablaDeSegmentoActualizada = list_map(tablaDeSegmentoActualizada,adapter_segmento_memoria_kernel);
                    pthread_mutex_unlock(&mutexListaDeSegmento);

                    t_buffer* bufferTablaDeSegmentoActualizada = buffer_create();
                    buffer_pack_segmento_list(bufferTablaDeSegmentoActualizada, tablaDeSegmentoActualizada);
                    stream_send_buffer(socketKernel, HANDSHAKE_ok_continue,bufferTablaDeSegmentoActualizada);
                    buffer_destroy(bufferTablaDeSegmentoActualizada);
               
                    //log_info(memoriaLogger, "\e[1;93mSe crea nuevo segmento con id [%i] y tamanio [%i]\e[0m", id_de_segmento, tamanio_de_segmento);
                
                    t_list* tablaDeSegmentoSolic = obtener_tabla_de_segmentos_por_pid(pid);


                }

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
            
                uint32_t retardoInstruccion = memoria_config_get_retardo_memoria(memoriaConfig);
                intervalo_de_pausa(retardoInstruccion);

                pthread_mutex_lock(&mutexListaDeSegmento);
                eliminar_segmento_memoria(segABorrar);
                pthread_mutex_unlock(&mutexListaDeSegmento);

                log_info(memoriaLogger, "Borramos el segmento <%i> del proceso <%i>", id_de_segmento, pid);
                
                pthread_mutex_lock(&mutexListaDeSegmento);
                t_list* tablaDeSegmentoActualizada = listaDeSegmentos;
                tablaDeSegmentoActualizada = list_filter(tablaDeSegmentoActualizada,segmentos_validez_1);
                tablaDeSegmentoActualizada = list_map(tablaDeSegmentoActualizada,adapter_segmento_memoria_kernel);
                mostrar_lista_segmentos(listaDeSegmentos);
                pthread_mutex_unlock(&mutexListaDeSegmento);


                t_buffer* bufferTablaDeSegmentoActualizada = buffer_create();
                buffer_pack_segmento_list(bufferTablaDeSegmentoActualizada, tablaDeSegmentoActualizada);
                stream_send_buffer(socketKernel, HANDSHAKE_ok_continue,bufferTablaDeSegmentoActualizada);
                buffer_destroy(bufferTablaDeSegmentoActualizada);


                break;
            }
            case HEADER_proceso_terminado: {
                uint32_t pid;
                buffer_unpack(buffer, &pid, sizeof(pid));
                pthread_mutex_lock(&mutexListaDeSegmento);
                liberar_tabla_segmentos(pid);
                pthread_mutex_unlock(&mutexListaDeSegmento);
                stream_send_empty_buffer(socketKernel, HANDSHAKE_ok_continue);
                log_info(memoriaLogger,RED STRIKETHROUGH "Eliminación de Proceso PID: <%i>", pid);
                mostrar_lista_segmentos(listaDeSegmentos);

            }
            default:
                //exit(-1);
                break;
        }
        pthread_mutex_unlock(&mutexMemoriaData);
        buffer_destroy(buffer);
    }
}

