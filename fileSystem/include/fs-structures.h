#ifndef FS_STRUCTURES_H
#define FS_STRUCTURES_H

//////////////////////// BIBLOTECAS BASICAS /////////////////
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <math.h>

//////////////////////// BIBLOTECAS COMMONS /////////////////
#include <commons/log.h>
#include <commons/config.h>
#include <commons/bitarray.h>


/////////////////////// ESTRUCTURA PARA FILESYSTEM ////////////////////////

typedef struct {
    char* ip_memoria;
    char* puerto_memoria;
    char* puerto_escucha;
    char* superbloque_path;
    char* bitmap_path;
    char* bloques_path;
    char* fcb_path;
    double retardo_accesos;
    
    int socket_memoria;
    int socket_kernel;

    uint32_t block_size;
    uint32_t block_count;

    char* nombre_archivo;
    uint32_t tamanio_archivo;
    uint32_t puntero_directo;
    uint32_t puntero_indirecto;
} t_filesystem;

typedef struct {
    uint32_t block_size;
    uint32_t block_count;
} t_superbloque;

/////////////////////// ESTRUCTURA PARA EL FCB ////////////////////////

typedef struct {
    char* nombre_archivo;
    uint32_t tamanio_archivo;
    uint32_t puntero_directo;
    uint32_t puntero_indirecto;
} t_fcb;



////////////////////////// DEFINICION DE LAS FUNCIONES ////////////////

void cargar_t_filesystem(t_config*, t_config*, t_config*, t_filesystem*);
void levantar_bitmap(t_filesystem*, t_log*);
void crear_estructuras_administrativas(t_filesystem*, t_log*);



#endif