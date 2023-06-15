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
#include <../../utils/src/utils/commons-nuestras.h>

#include <stdio.h>
#include "memoria-config.h"

typedef struct {
    int segmento_id; 
    uint32_t* limite; //tamanio
    uint32_t* base;
    int pid; 
    int validez;
} Segmento;


typedef struct {
    t_list* tablaDeSegmentos;
    int pid;
} Procesos;

#include "memoria.h"

int segmento_get_id(Segmento*);
void segmento_set_id(Segmento* , int );
uint32_t* segmento_get_limite(Segmento* );
void segmento_set_limite(Segmento* , uint32_t* );
uint32_t* segmento_get_base(Segmento* );
void segmento_set_base(Segmento* , uint32_t* );
int segmento_get_pid(Segmento* );
void segmento_set_pid(Segmento* , int );
int segmento_get_bit_validez(Segmento* );
void segmento_set_bit_validez(Segmento* , int );

void inicializar_memoria();
Segmento* crear_segmento(int );
void inicializar_estructuras();
void eliminar_segmento(Segmento* );


//////////////////////// PROCESOS ////////////////////////

/**
 * @brief Crea un nuevo proceso con el ID especificado.
 * @param pid El ID del proceso a crear.
 * @return Un puntero al proceso creado.
 */ 
Procesos* crear_proceso(int pid);

/**
 * @brief Obtiene un proceso por su ID.
 * @param pid_victima El ID del proceso a buscar.
 * @return Un puntero al proceso encontrado o NULL si no se encontró.
 */
Procesos* obtener_proceso_por_pid(int pid_victima);


//////////////////////// SEGMENTO ////////////////////////

/**
 * @brief Obtiene un segmento de un proceso por su ID.
 * @param proceso El proceso del cual se desea obtener el segmento.
 * @param id_victima El ID del segmento a buscar.
 * @return Un puntero al segmento encontrado o NULL si no se encontró.
 */
Segmento* obtener_segmento_por_id(Procesos* proceso, int id_victima);

/**
 * @brief Desencola y retorna un segmento de un proceso por su ID.
 * @param proceso El proceso del cual se desea desencolar el segmento.
 * @param id_segmento El ID del segmento a desencolar.
 * @return Un puntero al segmento desencolado o NULL si no se encontró.
 */
Segmento* desencolar_segmento_por_id(Procesos* proceso, int id_segmento);


#endif