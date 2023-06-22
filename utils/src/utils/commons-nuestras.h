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

#endif