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

    int abrir_archivo(char* nombre_archivo, t_filesystem* fs);


////////////////////////// F_CREATE ////////////////

    int crear_archivo(char* nombre_archivo, t_filesystem* fs);


////////////////////////// F_TRUNCATE ////////////////

    int truncar_archivo(char* nombre_archivo, uint32_t nuevo_tamanio, t_filesystem* fs);
    t_fcb* ampliar_tamanio_archivo(char* nombre_archivo, uint32_t nuevo_tamanio, t_filesystem* fs, int posicion_fcb);
    int puede_ampliar_tamanio(t_fcb* fcb_a_ampliar, uint32_t block_size, uint32_t nuevo_tamanio, uint32_t fcb_tamanio_actual);
    t_fcb* reducir_tamanio_archivo(char* nombre_archivo, uint32_t nuevo_tamanio, t_filesystem* fs, int posicion_fcb);


////////////////////////// F_READ ////////////////

    int leer_archivo(char* nombre_archivo, uint32_t direccion_fisica, uint32_t cant_bytes_a_leer, uint32_t puntero_proceso, t_filesystem* fs);


////////////////////////// F_WRITE ////////////////

    int escribir_archivo(char* nombre_archivo, uint32_t direccion_fisica, uint32_t cant_bytes_a_escribir, uint32_t puntero_proceso, t_filesystem* fs);


////////////////////////// ESPERAR KERNEL ////////////////

    int fs_escuchando_en(int servidor_fs, t_filesystem* fs);


////////////////////////// OTRAS ////////////////

    int devolver_posicion_fcb_en_la_lista(char* nombre_archivo);
    void devolver_cantidad_bytes_en_array(uint32_t cantidad_bytes, uint32_t* array_bytes, uint32_t block_size);


#endif