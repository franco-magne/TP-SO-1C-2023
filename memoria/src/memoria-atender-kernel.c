#include <../include/memoria-atender-kernel.h>
#include <../include/memoria-estructuras.h>

extern pthread_mutex_t mutexMemoriaData;
extern t_MemoriaData *memoriaData;

bool puedo_crear_proceso(uint32_t tamanio, t_MemoriaData* memoriaData){
    bool respuesta;
    //respuesta = tamanio <= memoriaData->tamMaxDefxCPU && hayEspacioLibre(id_proceso)  
                    //hayEspacioLibre(id_proceso) seria para asociar el proceso con su respectiva tabla de segmentos
    return respuesta;
}

void* atender_peticiones_kernel(void* socketKernel) {
    int socket = *(int*)socketKernel;
    uint32_t header, nroTablaNivel1;
    for (;;) {
        header = stream_recv_header(socket);
        pthread_mutex_lock(&mutexMemoriaData);
        t_buffer* buffer = buffer_create();
        stream_recv_buffer(socket, buffer);
        switch (header) {
            //case HEADER_solicitud_tabla_paginas: {  //Sera solic_tabla_segmentos o proceso_nuevo??
            case HEADER_solicitud_tabla_segmentos: {
                /*log_info(memoriaData->memoriaLogger, "\e[1;93mSe crea nuevo proceso\e[0m");
                uint32_t tamanio;
                buffer_unpack(buffer, &tamanio, sizeof(tamanio));
                if (puedo_crear_proceso(tamanio, memoriaData)) {
                    nroTablaNivel1 = __crear_nuevo_proceso(tamanio, memoriaData);
                    t_buffer* buffer_rta = buffer_create();
                    buffer_pack(buffer_rta, &nroTablaNivel1, sizeof(nroTablaNivel1));
                    stream_send_buffer(socket, HANDSHAKE_ok_continue, buffer_rta);
                    buffer_destroy(buffer_rta);
                    log_info(memoriaData->memoriaLogger, "Se asigno correctamente una tabla de nivel 1 con ID [%d] y tamaño [%d]", nroTablaNivel1, tamanio);
                } else {
                    stream_send_empty_buffer(socket, HEADER_error);
                    log_error(memoriaData->memoriaLogger, "No se pudo asignar tabla de nivel 1 con tamaño [%d]", tamanio);
                }
                buffer_destroy(buffer);*/
                log_info(memoriaData->memoriaLogger, "\e[1;93mSe crea nuevo proceso\e[0m");
                break;
            }
            /////Suspendido no hay mas
            /*case HEADER_proceso_suspendido:
                log_info(memoriaData->memoriaLogger, "\e[1;93mSe suspende proceso\e[0m");
                buffer_unpack(buffer, &nroTablaNivel1, sizeof(nroTablaNivel1));
                if (esta_suspendido(nroTablaNivel1, memoriaData)) {
                    log_info(memoriaData->memoriaLogger, "\e[1;92mYa se encuentra suspendido el proceso [%d]\e[0m", nroTablaNivel1);
                } else {
                    log_info(memoriaData->memoriaLogger, "\e[1;92mSe suspendio el proceso [%d]\e[0m", nroTablaNivel1);
                    suspender_proceso(nroTablaNivel1, memoriaData);
                }
                stream_send_empty_buffer(socket, HANDSHAKE_ok_continue);
                buffer_destroy(buffer);
                break;
            */
            case HEADER_proceso_terminado:
                log_info(memoriaData->memoriaLogger, "\e[1;93mSe finaliza proceso\e[0m");
                /*buffer_unpack(buffer, &nroTablaNivel1, sizeof(nroTablaNivel1));
                __eliminar_proceso(nroTablaNivel1, memoriaData);
                log_info(memoriaData->memoriaLogger, "Se finalizó tabla de nivel 1 con ID [%d]", nroTablaNivel1);
                stream_send_empty_buffer(socket, HANDSHAKE_ok_continue);
                buffer_destroy(buffer);*/
                break;
            case HEADER_createSegment:{
                log_info(memoriaData->memoriaLogger, "Se crea el segmento");
                /*if(hayEspacioLibre && elEspacioEsContiguo) {
                    log_info(memoriaData->memoriaLogger, "Cree el segmento");
                    stream_send_buffer(socketKernel, HEADER_base_segmento_creado, buffer);
                    }
                else if(hayEspacioLibre && !elEspacioEsContiguo){
                    log_info(memoriaData->memoriaLogger, "Solic a kernel compactacion");
                }
                else{
                    log_info(memoriaData->memoriaLogger, "Informo a kernel OutOfMemory");
                }*/
            }
            case HEADER_deleteSegment:{
                Segmento* segmento;
                buffer_unpack(socketKernel, segmento, sizeof(segmento));
                segmento->base = -1; //ASI PODRIA SEÑALAR Q EL SEGMENTO ESTA VACIO????
                log_info(memoriaData->memoriaLogger, "Se elimino el segmento id: [%d]", segmento->segmento_id);
            }
            case HEADER_compactacion:{
                log_info(memoriaData->memoriaLogger, "Se hizo la compactacion de la tabla de segmentos");
            }
            default:
                exit(-1);
                break;
        }
        pthread_mutex_unlock(&mutexMemoriaData);
    }
    return NULL;
}