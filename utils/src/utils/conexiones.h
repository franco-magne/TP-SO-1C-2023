#ifndef CONEXIONES_H
#define CONEXIONES_H


#include <stdio.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef enum {
    
    HEADER_error,
    HEADER_pid,
    HEADER_lista_instrucciones,
    HEADER_interrumpir_ejecucion,
    HEADER_chequeo_DF,
    HEADER_memoria_insuficiente,
    HEADER_pcb_a_ejecutar,
    HEADER_proceso_bloqueado,
    HEADER_proceso_desalojado,
    HEADER_proceso_terminado,
    HEADER_solicitud_tabla_paginas_segmentos,
    HEADER_proceso_pedir_recurso,
    HEADER_proceso_devolver_recurso,
    HEADER_create_segment,
    HEADER_delete_segment,
    HEADER_f_open,
    HEADER_f_create,
    HEADER_f_close,
    HEADER_f_seek,
    HEADER_f_truncate,
    HEADER_f_read,
    HEADER_f_write,
    HEADER_move_in,
    HEADER_move_out,
    HEADER_iniciar_proceso,
    HEADER_tabla_segmentos,
    HEADER_Compactacion,
    HEADER_Compactacion_finalizada,
    HEADER_Segmentation_fault

} t_header;

typedef enum {

    HANDSHAKE_consola,
    HANDSHAKE_kernel,
    HANDSHAKE_memoria,
    HANDSHAKE_cpu,
    HANDSHAKE_fileSystem,
    HANDSHAKE_interrupt,
    HANDSHAKE_dispatch,
    HANDSHAKE_ok_continue,
    HANDSHAKE_seg_muy_grande
    
} t_handshake;


int conectar_a_servidor(char* ip, char* puerto);
int iniciar_servidor(char* ip, char* puerto);
int esperar_cliente(int socket_servidor);
bool not(bool x);


#endif