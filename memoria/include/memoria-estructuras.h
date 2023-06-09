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

#define MEMORIA_CONFIG_UBICACION "config/memoria.config"
#define MEMORIA_LOG_UBICACION "logs/memoria.log"
#define MEMORIA_PROCESS_NAME "Memoria"


/*typedef struct {
    void* espacio_usuario; // Espacio de usuario
    int tamanio;
} Memoria;*/

typedef struct {
    int segmento_id; 
    uint32_t limite; //tamanio
    uint32_t base;
    int validez;
} Segmento;




/*
typedef struct {
    int base;
    int tamanio;
} EspacioLibre;  //lo calculo dinamicamente cuand creo segmento

typedef struct {
    EspacioLibre* espacios_libres;
} ListaEspacioLibre;
*/




#endif