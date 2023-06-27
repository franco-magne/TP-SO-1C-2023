#ifndef FS_STRUCTURES_H
#define FS_STRUCTURES_H

//////////////////////// BIBLOTECAS BASICAS /////////////////
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <math.h>
#include <dirent.h>
#include <errno.h>

//////////////////////// BIBLOTECAS COMMONS /////////////////
#include <commons/log.h>
#include <commons/config.h>
#include <commons/bitarray.h>
#include <commons/string.h>
#include <commons/collections/list.h>


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

    t_log* logger;
} t_filesystem;

/////////////////////// ESTRUCTURA PARA EL FCB ////////////////////////

typedef struct {
    char* nombre_archivo;
    char* tamanio_archivo;
    uint32_t puntero_directo;
    uint32_t puntero_indirecto;

    t_list* bloques;
    t_config* fcb_config;
} t_fcb;



////////////////////////// DEFINICION DE LAS FUNCIONES ////////////////


////////////////////////// SUPERBLOQUE ////////////////

    void crear_superbloque_dat(t_filesystem*, t_config*);


////////////////////////// BITMAP ////////////////

    void levantar_bitmap(t_filesystem*);
    void crear_bitmap(t_filesystem*);
    void abrir_bitmap(t_filesystem*);


////////////////////////// ARCHIVO DE BLOQUES ////////////////

    void levantar_archivo_de_bloques(t_filesystem*);
    void crear_archivo_de_bloques(t_filesystem*);
    void abrir_archivo_de_bloques(t_filesystem*);
    uint32_t buscar_bloque_libre(t_filesystem*, uint32_t*);
    void liberar_bloque(t_filesystem*, uint32_t*);


////////////////////////// FCB ////////////////

    t_fcb* crear_fcb_inexistente(char*, t_filesystem*);


////////////////////////// DIRECTORIO DEL FCB  ////////////////

    void levantar_fcbs_del_directorio(t_filesystem*, t_list*);
    int el_directorio_fcb_esta_vacio(t_filesystem*);
    void crear_fcbs_del_directorio(t_filesystem*, t_list*);
    void crear_directorios(t_filesystem*);


////////////////////////// OTRAS ////////////////

    void cargar_t_filesystem(t_config*, t_config*, t_filesystem*);
    char* devolver_fcb_path_config(char*, char*);
    void cerrar_archivos();


#endif