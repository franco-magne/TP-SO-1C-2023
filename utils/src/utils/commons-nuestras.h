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


FILE* abrir_archivo(const char* pathArchivo, const char* mode, t_log* moduloLogger);
int config_init(void* moduleConfig, char* pathToConfig, t_log* moduleLogger, void (*config_initializer)(void* moduleConfig, t_config* tempConfig));
void intervalo_de_pausa(uint32_t duracionEnMilisegundos);

#endif