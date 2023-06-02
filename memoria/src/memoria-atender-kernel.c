#include <../include/memoria-atender-kernel.h>
#include <../include/memoria-estructuras.h>

//extern pthread_mutex_t mutexMemoriaData;
t_log *memoriaLogger;
t_config *memoriaConfig;

bool puedo_crear_proceso(uint32_t tamanio){
    bool respuesta=1;
    //respuesta = tamanio <= tamMaxDefxCPU && hayEspacioLibre(id_proceso)  
                    //hayEspacioLibre(id_proceso) seria para asociar el proceso con su respectiva tabla de segmentos
    return respuesta;   
}


void* atender_peticiones_kernel(void* socketKernel) {
    int socket = *(int*)socketKernel;
    uint32_t header;
    for (;;) {
        header = stream_recv_header(socket);
        //pthread_mutex_lock(&mutexMemoriaData);
        t_buffer* buffer = buffer_create();
        stream_recv_buffer(socket, buffer);
        switch (header) {
            case HEADER_solicitud_tabla_segmentos: {
                /*log_info(memoriaLogger, "\e[1;93mSe crea nuevo proceso\e[0m");
                uint32_t tamanioTdeSeg;
                buffer_unpack(buffer, &tamanioTdeSeg, sizeof(tamanio));
                if (puedo_crear_proceso(tamanioTdeSeg, memoriaData)) {
    
                    t_buffer* buffer_rta = buffer_create();
                    buffer_pack(buffer_rta, tablaDeSegmentos??, sizeof(tablaDeSegmentos)); DUDAS???

                    stream_send_buffer(socket, HANDSHAKE_ok_continue, buffer_rta);
                    buffer_destroy(buffer_rta);
                    log_info(memoriaLogger, "Se asigno correctamente una tabla de segmentos con tamaño [%d]", tamanioTdeSeg);
                } else {
                    stream_send_empty_buffer(socket, HEADER_error);
                    log_error(memoriaLogger, "No se pudo asignar tabla de segmentos con tamaño [%d]", tamanioTdeSeg);
                }
                buffer_destroy(buffer);*/
                log_info(memoriaLogger, "\e[1;93mSe crea nuevo proceso\e[0m");
                break;
            }
            case HEADER_proceso_terminado:
                log_info(memoriaLogger, "\e[1;93mSe finaliza proceso\e[0m");
                /*buffer_unpack(buffer, tablaDeSegmentos, sizeof(tablaDeSegmentos));
                __eliminar_proceso(tablaDeSegmentos, memoriaData);
                log_info(memoriaLogger, "Se finalizó tabla de Segmentos); //tiene un ID??
                stream_send_empty_buffer(socket, HANDSHAKE_ok_continue);
                buffer_destroy(buffer);*/
                break;
            case HEADER_createSegment:{
                log_info(memoriaLogger, "Se crea el segmento");
                /*if(hayEspacioLibre && elEspacioEsContiguo) {
                    log_info(memoriaLogger, "Cree el segmento");
                    stream_send_buffer(socketKernel, HEADER_base_segmento_creado, buffer);
                    }
                else if(hayEspacioLibre && !elEspacioEsContiguo){
                    log_info(memoriaLogger, "Solic a kernel compactacion");
                }
                else{
                    log_info(memoriaLogger, "Informo a kernel OutOfMemory");
                }*/
            }
            case HEADER_deleteSegment:{
                /*Segmento* segmento;
                buffer_unpack(socketKernel, segmento, sizeof(segmento));
                segmento->base = -1; //ASI PODRIA SEÑALAR Q EL SEGMENTO ESTA VACIO????*/
                log_info(memoriaLogger, "Se elimino el segmento id: ");
            }
            case HEADER_compactacion:{
                log_info(memoriaLogger, "Se hizo la compactacion de la tabla de segmentos");
            }
            default:
                exit(-1);
                break;
        }
        //pthread_mutex_unlock(&mutexMemoriaData);
    }
    return NULL;
}
