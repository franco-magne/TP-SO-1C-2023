#ifndef COMMONS_NUESTRAS_H
#define COMMONS_NUESTRAS_H

#include <commons/collections/list.h>
#include <commons/log.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>


FILE* abrir_archivo(const char* pathArchivo, const char* mode, t_log* moduloLogger);

#endif