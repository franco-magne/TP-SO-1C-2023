#ifndef COMMONS_NUESTRAS_H
#define COMMONS_NUESTRAS_H

#include <commons/collections/list.h>
#include <commons/config.h>

#include <commons/log.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include "instrucciones.h"
#include "serializacion.h"

typedef struct{
 uint32_t tamanio_de_segmento;
 uint32_t id_de_segmento;
 uint32_t base_del_segmento;
 bool victima;
 uint32_t pid;

}t_segmento;

FILE* abrir_archivo(const char* pathArchivo, const char* mode, t_log* moduloLogger);
int config_init(void* moduleConfig, char* pathToConfig, t_log* moduleLogger, void (*config_initializer)(void* moduleConfig, t_config* tempConfig));
void intervalo_de_pausa(uint32_t duracionEnMilisegundos);
int size_recurso_list(char **);
bool contains(char** list, char* object );
int position_in_list(char** list, char* object );
int list_get_index(t_list* list, bool (*cutting_condition)(void*, void*), void* target);
void set_timespec(struct timespec* timespec);
double obtener_diferencial_de_tiempo_en_milisegundos(struct timespec end, struct timespec start);
bool es_el_ultimo_elemento(t_list* lista, t_link_element* elemento);
t_list* list_filter_ok(t_list* lista, bool (*condition)(void*, void*), void* argumento);
t_list* buffer_unpack_segmento_list(t_buffer* buffer);
t_segmento* buffer_unpack_segmento(t_buffer* buffer);
void buffer_pack_segmento(t_buffer* buffer, t_segmento* segmento);
void buffer_pack_segmento_list(t_buffer* buffer, t_list* lista_segmentos);
t_segmento* segmento_create(uint32_t id_de_segmento, uint32_t tamanio_de_segmento);




void inicio_kernel();
void inicio_cpu();


#endif