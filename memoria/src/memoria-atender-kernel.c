#include <../include/memoria-atender-kernel.h>
//#include <../include/memoria-estructuras.h>

pthread_mutex_t mutexMemoriaData; //extern
extern t_log *memoriaLogger;
//t_config *memoriaConfig;

bool puedo_crear_proceso(uint32_t tamanio){
    bool respuesta=1;
    //respuesta = tamanio <= tamMaxDefxCPU && hayEspacioLibre(id_proceso)  
                    //hayEspacioLibre(id_proceso) seria para asociar el proceso con su respectiva tabla de segmentos
    return respuesta;   
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
        
                log_info(memoriaLogger, "\e[1;93mSe crea nuevo segmento con id [%i] y tamanio [%i]\e[0m", id_de_segmento, tamanio_de_segmento);

                sleep(5);
                stream_send_empty_buffer(socketKernel, HANDSHAKE_ok_continue);
            
                log_info(memoriaLogger,"socket kernel -> 2-<%i> ", socketKernel);

                //buffer_destroy(buffer);
                break;
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