#ifndef CONEXIONES_H
#define CONEXIONES_H


#include <stdio.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>



typedef enum {
    
    HEADER_error,
    HEADER_pid,
    HEADER_lista_instrucciones,
    HEADER_interrumpir_ejecucion,
    HEADER_marco,
    HEADER_memoria_insuficiente,
    HEADER_pcb_a_ejecutar,
    HEADER_proceso_bloqueado,
    HEADER_proceso_desalojado,
    HEADER_proceso_terminado,
    //HEADER_movin,
    //HEADER_movout,
    HEADER_solicitud_tabla_segmentos,
    //HEADER_page_fault,
    //HEADER_page_fault_resuelto,
    HEADER_instruccion_io_teclado,
    HEADER_instruccion_io_pantalla,
    HEADER_createSegment,
    HEADER_deleteSegment,
    HEADER_compactacion
} t_header;

typedef enum {

    HANDSHAKE_consola,
    HANDSHAKE_kernel,
    HANDSHAKE_memoria,
    HANDSHAKE_cpu,
    HANDSHAKE_interrupt,
    HANDSHAKE_dispatch,
    HANDSHAKE_ok_continue
    
} t_handshake;


int conectar_a_servidor(char* ip, char* puerto);
int iniciar_servidor(char* ip, char* puerto);
int esperar_cliente(int socket_servidor);



#endif