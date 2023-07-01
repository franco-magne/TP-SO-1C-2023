#ifndef FS_ATENDER_KERNEL_H
#define FS_ATENDER_KERNEL_H

//////////////////////// BIBLOTECAS BASICAS /////////////////
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>

//////////////////////// BIBLOTECAS COMMONS /////////////////
#include <commons/log.h>
#include <commons/config.h>

//////////////////////////////// BIBLOTECA UTILS NUESTRA //////////////
#include <../../utils/src/utils/serializacion.h>
#include <../../utils/src/utils/conexiones.h>
#include "../include/fs-structures.h"



////////////////////////// DEFINICION DE LAS FUNCIONES ////////////////


////////////////////////// ATENDER KERNEL ////////////////

    void atender_kernel(t_filesystem*);


////////////////////////// F_OPEN ////////////////

    int abrir_archivo(char*, t_filesystem*);


////////////////////////// F_CREATE ////////////////

    int crear_archivo(char*, t_filesystem*);


////////////////////////// F_TRUNCATE ////////////////

    int truncar_archivo(char*, uint32_t, t_filesystem*);
    t_fcb* ampliar_tamanio_archivo(char*, uint32_t, t_filesystem*, int);
    t_fcb* reducir_tamanio_archivo(char*, uint32_t, t_filesystem*, int);


////////////////////////// ESPERAR KERNEL ////////////////

    int fs_escuchando_en(int, t_filesystem*);


#endif