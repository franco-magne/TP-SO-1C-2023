#ifndef FS_ATENDER_KERNEL_H
#define FS_ATENDER_KERNEL_H

//////////////////////// BIBLOTECAS BASICAS /////////////////
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <inttypes.h>

//////////////////////// BIBLOTECAS COMMONS /////////////////
#include <commons/log.h>
#include <commons/config.h>

//////////////////////////////// BIBLOTECA UTILS NUESTRA //////////////
#include <../../utils/src/utils/commons-nuestras.h>
#include <../../utils/src/utils/serializacion.h>
#include <../../utils/src/utils/conexiones.h>
#include "../include/fs-structures.h"



////////////////////////// DEFINICION DE LAS FUNCIONES ////////////////


/*------------------------------------------------------------------------- ATENDER KERNEL ----------------------------------------------------------------------------- */

    int atender_kernel(t_filesystem*);


/*------------------------------------------------------------------------- F_OPEN ----------------------------------------------------------------------------- */

    int abrir_archivo_fs(char* nombre_archivo, t_filesystem* fs);


/*------------------------------------------------------------------------- F_CREATE ----------------------------------------------------------------------------- */

    int crear_archivo(char* nombre_archivo, t_filesystem* fs);


/*------------------------------------------------------------------------- F_TRUNCATE ----------------------------------------------------------------------------- */

    int truncar_archivo(char* nombre_archivo, uint32_t nuevo_tamanio, t_filesystem* fs);
    t_fcb* ampliar_tamanio_archivo(char* nombre_archivo, uint32_t nuevo_tamanio, t_filesystem* fs, int posicion_fcb);
    int puede_ampliar_tamanio(t_fcb* fcb_a_ampliar, uint32_t block_size, uint32_t nuevo_tamanio, uint32_t fcb_tamanio_actual);
    t_fcb* reducir_tamanio_archivo(char* nombre_archivo, uint32_t nuevo_tamanio, t_filesystem* fs, int posicion_fcb);
    void persistir_fcb_config(t_fcb* fcb_truncado);


/*------------------------------------------------------------------------- F_READ ----------------------------------------------------------------------------- */

    void leer_archivo(char* nombre_archivo, uint32_t base_fisica, uint32_t desplazamiento_fisico, uint32_t cant_bytes_a_leer, uint32_t puntero_proceso, t_filesystem* fs);
    char* leer_archivo_bytes_menor_a_block_size(uint32_t cant_bytes, uint32_t puntero, uint32_t base_fisica, uint32_t desplazamiento_fisico, t_fcb* fcb_a_leer, t_filesystem* fs);
    char* leer_archivo_bytes_mayor_a_block_size(uint32_t cant_bytes, uint32_t puntero, uint32_t base_fisica, uint32_t desplazamiento_fisico, t_fcb* fcb_a_leer, t_filesystem* fs);
    void enviar_informacion_a_memoria(uint32_t base_fisica, uint32_t desplazamiento_fisico, char* cadena_leida, uint32_t cant_bytes_a_leer, t_filesystem* fs);


/*------------------------------------------------------------------------- F_WRITE ----------------------------------------------------------------------------- */

    void escribir_archivo(char* nombre_archivo, uint32_t base_fisica, uint32_t desplazamiento_fisico, uint32_t cant_bytes_a_escribir, uint32_t puntero_proceso, t_filesystem* fs);
    void escribir_archivo_bytes_menor_a_block_size(uint32_t cant_bytes, uint32_t puntero, uint32_t base_fisica, t_fcb* fcb_a_escribir, t_filesystem* fs, char* respuesta_memoria);
    void escribir_archivo_bytes_mayor_a_block_size(uint32_t cant_bytes, uint32_t puntero, uint32_t base_fisica, t_fcb* fcb_a_escribir, t_filesystem* fs, char* respuesta_memoria);
    char* pedir_informacion_a_memoria(uint32_t base_fisica, uint32_t desplazamiento_fisico, uint32_t cant_bytes_necesarios, t_filesystem* fs);


/*------------------------------------------------------------------------- ESPERAR KERNEL ----------------------------------------------------------------------------- */

    int fs_escuchando_en(int servidor_fs, t_filesystem* fs);


/*------------------------------------------------------------------------- OTRAS ----------------------------------------------------------------------------- */

    int devolver_posicion_fcb_en_la_lista(char* nombre_archivo);
    void convertir_cantidad_bytes_en_array(uint32_t cantidad_bytes, uint32_t* array_bytes, uint32_t block_size);


#endif