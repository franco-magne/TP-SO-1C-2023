#ifndef FS_ATENDER_KERNEL_H
#define FS_ATENDER_KERNEL_H

//////////////////////// BIBLOTECAS BASICAS /////////////////
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>

//////////////////////// BIBLOTECAS COMMONS /////////////////
#include <commons/log.h>
#include <commons/config.h>

//////////////////////////////// BIBLOTECA UTILS NUESTRA //////////////
#include <../../utils/src/utils/serializacion.h>
#include <../../utils/src/utils/conexiones.h>
#include "../include/fs-structures.h"



////////////////////////// DEFINICION DE LAS FUNCIONES ////////////////

void atender_kernel(t_filesystem*);
int fs_escuchando_en(int, t_filesystem*); 
int buscar_archivo(char*); 
int abrir_archivo(char*, t_filesystem*);
int truncar_archivo(char*, uint32_t);

#endif