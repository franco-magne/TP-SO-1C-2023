#include <../include/memoria-atender-kernel.h>


pthread_mutex_t mutexMemoriaData; //extern
pthread_mutex_t mutexTamMemoriaActual = PTHREAD_MUTEX_INITIALIZER; //extern

extern t_log *memoriaLogger;
extern t_memoria_config* memoriaConfig;
extern uint32_t tamActualMemoria;
extern t_estado* tabla_segmentos; 
extern Segmento* segCompartido;

extern t_list* listaDeProcesos;

//t_config *memoriaConfig;

bool puedo_crear_proceso_o_segmento(uint32_t tamanio){
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

                if(!puedo_crear_proceso_o_segmento(memoria_config_get_tamanio_segmento_0(memoriaConfig))){ //+sizeof(*tabla_de_segmentos)
                    log_error(memoriaLogger, "No se pudo crear el proceso, no hay espacio en memoria");
                }
                inicializar_estructuras();
                // agrego pid como param a crear_proceso 
                Procesos* proceso = crear_proceso(pid); //proceso inicilizado con SegCompartido en la tabla de segmentos
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
                int pid;
                
                buffer_unpack(buffer, &id_de_segmento, sizeof(id_de_segmento));
                buffer_unpack(buffer, &tamanio_de_segmento, sizeof(tamanio_de_segmento));
                buffer_unpack(buffer, &pid, sizeof(pid));

                log_info(memoriaLogger,"tamanio de segmento <%i> y tamActualMemoria <%i>", tamanio_de_segmento, tamActualMemoria);

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
                Procesos* unProceso = obtener_proceso_por_pid(pid);

                pthread_mutex_lock(&mutexTamMemoriaActual);
                tamActualMemoria -= tamanio_de_segmento;
                encolar_segmento_atomic(unProceso->tablaDeSegmentos,unSegmento);
                pthread_mutex_unlock(&mutexTamMemoriaActual);

                log_info(memoriaLogger, "\e[1;93mSe crea nuevo segmento con id [%i] y tamanio [%i]\e[0m", id_de_segmento, tamanio_de_segmento);

                stream_send_empty_buffer(socketKernel, HANDSHAKE_ok_continue);
                
                if( (list_size(unProceso->tablaDeSegmentos) == 4)){
                for(int i = 0; i<(list_size(unProceso->tablaDeSegmentos)); i++ ){
                    Segmento* x = desencolar_segmento_primer_segmento_atomic(unProceso);
                    log_info(memoriaLogger, "id_segmento <%i> : ", x->segmento_id );
                }
                }
                //buffer_destroy(buffer);
                break;
            }
            case HEADER_delete_segment:{
                uint32_t id_de_segmento;
                int pid;
                buffer_unpack(buffer, &id_de_segmento, sizeof(id_de_segmento));
                buffer_unpack(buffer, &pid, sizeof(pid));

                log_info(memoriaLogger,"Segmento a eliminar <%i> ", id_de_segmento);

                Procesos* miProceso = obtener_proceso_por_pid(pid);
                Segmento* segABorrar = desencolar_segmento_por_id(miProceso, id_de_segmento);
                free(segABorrar); 
                sumar_memoriaRecuperada_a_tamMemoriaActual(segABorrar->limite); //limite es el tamanio???
                log_info(memoriaLogger, "Borramos el segmento <%i> del proceos <%i>", id_de_segmento, pid);

                t_buffer* buffer_rta = buffer_create();
                buffer_pack(buffer_rta, miProceso->tablaDeSegmentos, sizeof(miProceso->tablaDeSegmentos)); //tablaDeSegmentos Actualizada
                stream_send_empty_buffer(socketKernel, HANDSHAKE_ok_continue);
                stream_send_buffer(socketKernel, HEADER_tabla_segmentos, buffer_rta);
                //buffer_destroy(buffer);
                break;
            }
            case HEADER_proceso_terminado: {
                int pid;
                buffer_unpack(buffer, &pid, sizeof(pid));
                liberar_tabla_segmentos(pid);
                stream_send_empty_buffer(socketKernel, HEADER_proceso_terminado);
                //buffer_destroy(buffer);
            }
            default:
                //exit(-1);
                break;
        }
        pthread_mutex_unlock(&mutexMemoriaData);
    }
}

