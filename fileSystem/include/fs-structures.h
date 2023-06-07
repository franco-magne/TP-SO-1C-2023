#ifndef FS_STRUCTURES_H
#define FS_STRUCTURES_H

//////////////////////// BIBLOTECAS BASICAS /////////////////
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

//////////////////////// BIBLOTECAS COMMONS /////////////////
#include <commons/log.h>
#include <commons/config.h>


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
} t_filesystem;



////////////////////////// DEFINICION DE LAS FUNCIONES ////////////////

void cargar_t_filesystem(t_config*, t_filesystem*);



#endif