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
    uint32_t limite;
    uint32_t base;
    uint32_t tamanio;
    int pid; 
    int validez;
    char* contenido;
} Segmento;

#include "memoria.h"

int segmento_get_id(Segmento*);
void segmento_set_id(Segmento* , int );
uint32_t segmento_get_limite(Segmento* );
void segmento_set_limite(Segmento* , uint32_t );
uint32_t segmento_get_base(Segmento* );
void segmento_set_base(Segmento* , uint32_t );
uint32_t segmento_get_tamanio(Segmento* );
void segmento_set_tamanio(Segmento* , uint32_t );
int segmento_get_pid(Segmento* );
void segmento_set_pid(Segmento* , int );
int segmento_get_bit_validez(Segmento* );
void segmento_set_bit_validez(Segmento* , int );
char* segmento_get_contenido(Segmento* un_segmento);
void segmento_set_contenido(Segmento* un_segmento, char* contenido);

void inicializar_memoria();
void inicializar_estructuras();


//////////////////////// SEGMENTO ////////////////////////
Segmento* crear_segmento(int tamSegmento);
bool es_el_segmento_victima_id(Segmento* unSegmento, Segmento* otroSegmento);

/**
 * @brief Obtiene un segmento de un proceso por su ID.
 * @param proceso El proceso del cual se desea obtener el segmento.
 * @param id_victima El ID del segmento a buscar.
 * @return Un puntero al segmento encontrado o NULL si no se encontró.
 */
Segmento* obtener_segmento_por_id(int pid_victima, int id_victima);

/**
 * @brief Desencola y retorna un segmento de un proceso por su ID.
 * @param proceso El proceso del cual se desea desencolar el segmento.
 * @param id_segmento El ID del segmento a desencolar.
 * @return Un puntero al segmento desencolado o NULL si no se encontró.
 */
Segmento* desencolar_segmento_por_id(int pid_segmento, int id_segmento);
Segmento* desencolar_segmento_primer_segmento_atomic();
void encolar_segmento_atomic(Segmento* targetSegmento);
t_list* obtener_tabla_de_segmentos_por_pid(int pid);
void modificarSegmento(uint32_t baseSegmento, Segmento* segNuevo);
Segmento* obtener_segmento_por_BASE(uint32_t base_segmento);

void sumar_memoriaRecuperada_a_tamMemoriaActual(uint32_t tamMemorRecuperada);
bool puedo_crear_proceso_o_segmento(uint32_t tamanio);
void restar_a_tamMemoriaActual(uint32_t memoriaARestar);


void liberar_tabla_segmentos(int pid);


#endif