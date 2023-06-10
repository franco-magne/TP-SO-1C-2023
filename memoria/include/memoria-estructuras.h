#ifndef MEMORIA_ESTRUCTURAS_H
#define MEMORIA_ESTRUCTURAS_H

#include <errno.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <sys/socket.h>
#include <../../utils/src/utils/conexiones.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <pthread.h>

#include <stdio.h>
#include "memoria-config.h"

typedef struct {
    int segmento_id; 
    uint32_t* limite; //tamanio
    uint32_t* base;
    int socket; 
    int validez;
} Segmento;

int segmento_get_id(Segmento*);
void segmento_set_id(Segmento* , int );
uint32_t* segmento_get_limite(Segmento* );
void segmento_set_limite(Segmento* , uint32_t* );
uint32_t* segmento_get_base(Segmento* );
void segmento_set_base(Segmento* , uint32_t* );
int segmento_get_socket(Segmento* );
void segmento_set_socket(Segmento* , int );
int segmento_get_bit_validez(Segmento* );
void segmento_set_bit_validez(Segmento* , int );

void inicializar_memoria();
Segmento* crear_segmento(int );
void inicializar_estructuras();
void eliminar_segmento(Segmento* );


#endif