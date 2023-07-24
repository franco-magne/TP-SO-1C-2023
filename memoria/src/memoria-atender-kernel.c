#include <../include/memoria-atender-kernel.h>


extern pthread_mutex_t mutexMemoriaData; 
extern pthread_mutex_t mutexListaDeSegmento;
extern t_log *memoriaLogger;
extern t_memoria_config* memoriaConfig;
extern Segmento* segCompartido;
extern void* memoriaPrincipal;

extern t_list* listaDeSegmentos; //la VERDADERA
int test = 1;
//t_config *memoriaConfig;



void atender_peticiones_kernel(int socketKernel) {
    uint8_t header;
    while (test) {
        header = stream_recv_header(socketKernel);
        pthread_mutex_lock(&mutexMemoriaData);
        t_buffer* buffer = buffer_create();
        stream_recv_buffer(socketKernel, buffer);

        switch (header) {
            case HEADER_create_segment: { 
                log_info(memoriaLogger, "\e[1;93mPETICION DE NUEVO SEGMENTO KERNEL\e[0m");


                uint32_t tamanio_de_segmento;  
                uint32_t id_de_segmento;
                int pid;
                buffer_unpack(buffer, &id_de_segmento, sizeof(id_de_segmento));
                buffer_unpack(buffer, &tamanio_de_segmento, sizeof(tamanio_de_segmento));
                buffer_unpack(buffer, &pid, sizeof(pid));

                t_list* tabla_segmentos_proceso = obtener_tabla_de_segmentos_por_pid(pid);

                if(list_is_empty(tabla_segmentos_proceso)){
                log_info(memoriaLogger, BOLDRED ITALIC UNDERLINE "Creación de Proceso: Creación de Proceso PID: "RESET BOLDYELLOW ITALIC UNDERLINE "<%i>",pid);
                }

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

                uint32_t retardoInstruccion = memoria_config_get_retardo_memoria(memoriaConfig);
                intervalo_de_pausa(retardoInstruccion);

                pthread_mutex_lock(&mutexListaDeSegmento);
                uint8_t memoriaRespuesta = administrar_nuevo_segmento(unSegmento);
                pthread_mutex_unlock(&mutexListaDeSegmento);

                if(memoriaRespuesta == HEADER_Compactacion){
                    sumar_memoriaRecuperada_a_tamMemoriaActual(tamanio_de_segmento);
                    stream_send_empty_buffer(socketKernel,HEADER_Compactacion);
                    uint8_t respuestaDeKernel = stream_recv_header(socketKernel);
                    stream_recv_empty_buffer(socketKernel);
                    if(respuestaDeKernel == HANDSHAKE_ok_continue){
                        log_info(memoriaLogger, MAGENTA ITALIC "SE INICIA LA COMPACTACION DE LA MEMORIA");
                        iniciar_compactacion();
                        log_info(memoriaLogger, MAGENTA ITALIC "SE FINALIZA LA COMPACTACION DE LA MEMORIA");
                        pthread_mutex_lock(&mutexListaDeSegmento);
                        mostrar_lista_segmentos_actualizados_por_compactacion();
                        pthread_mutex_unlock(&mutexListaDeSegmento);
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
               
                
                }

                mostrar_lista_segmentos(listaDeSegmentos);
                break;
            }
            case HEADER_delete_segment:{
                log_info(memoriaLogger, "\e[1;93mPETICION DE BORRALR SEGMENTO KERNEL\e[0m");
                uint32_t id_de_segmento;
                int pid;
                buffer_unpack(buffer, &id_de_segmento, sizeof(id_de_segmento));
                buffer_unpack(buffer, &pid, sizeof(pid));
                
                Segmento* segABorrar = crear_segmento(-1);
                segmento_set_id(segABorrar, id_de_segmento);
                segmento_set_pid(segABorrar, pid);
            
                uint32_t retardoInstruccion = memoria_config_get_retardo_memoria(memoriaConfig);
                intervalo_de_pausa(retardoInstruccion);

                pthread_mutex_lock(&mutexListaDeSegmento);
                eliminar_segmento_memoria(segABorrar);
                pthread_mutex_unlock(&mutexListaDeSegmento);

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
                log_info(memoriaLogger,RED STRIKETHROUGH "SE ELIMINA EL PROCESO CON PID: <%i>", pid);
                mostrar_lista_segmentos(listaDeSegmentos);
                break;
            }
            default:
                test = 0;
                break;
        }
        pthread_mutex_unlock(&mutexMemoriaData);
        buffer_destroy(buffer);
    }

}
