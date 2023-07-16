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
#include <inttypes.h>

//////////////////////// BIBLOTECAS COMMONS /////////////////
#include <commons/log.h>
#include <commons/config.h>
#include <commons/bitarray.h>
#include <commons/string.h>
#include <commons/collections/list.h>

//////////////////////////////// BIBLOTECA UTILS NUESTRA //////////////
#include <../../utils/src/utils/commons-nuestras.h>


/////////////////////// ESTRUCTURA PARA FILESYSTEM ////////////////////////

typedef struct {
    char* ip_escucha;
    char* ip_memoria;
    char* puerto_memoria;
    char* puerto_escucha;
    char* superbloque_path;
    char* bitmap_path;
    char* bloques_path;
    char* fcb_path;
    uint32_t retardo_accesos;
    
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


/*------------------------------------------------------------------------- SUPERBLOQUE ----------------------------------------------------------------------------- */

    void crear_superbloque_dat(t_filesystem* fs, t_config* superbloque);


/*------------------------------------------------------------------------- BITMAP ----------------------------------------------------------------------------- */

    void levantar_bitmap(t_filesystem* fs);
    void crear_bitmap(t_filesystem* fs);
    void abrir_bitmap(t_filesystem* fs);


/*------------------------------------------------------------------------- ARCHIVO DE BLOQUES ----------------------------------------------------------------------------- */

    void levantar_archivo_de_bloques(t_filesystem* fs);
    void crear_archivo_de_bloques(t_filesystem* fs);
    void abrir_archivo_de_bloques(t_filesystem* fs);
    void buscar_bloque_libre(t_filesystem* fs, uint32_t* puntero_bloque_libre);
    void liberar_bloque(t_filesystem* fs, uint32_t* puntero_bloque_a_liberar);
    void escribir_bloque_de_punteros_en_puntero_indirecto(uint32_t puntero_indirecto, uint32_t numero_puntero_X, uint32_t* puntero_a_escribir, uint32_t block_size);
    void liberar_puntero_del_bloque_de_punteros_en_puntero_indirecto(uint32_t puntero_indirecto, uint32_t posicion_ultimo_puntero, uint32_t block_size);
    char* leer_puntero_del_archivo_de_bloques(uint32_t puntero_acceder, uint32_t bytes_a_leer, t_filesystem* fs);
    void escribir_en_puntero_del_archivo_de_bloques(uint32_t puntero_acceder, uint32_t bytes_a_escribir, char* cadena_a_escribir, t_filesystem* fs);
    t_list* recuperar_bloque_de_punteros(uint32_t puntero_indirecto, int tamanio_archivo, uint32_t block_size);


/*------------------------------------------------------------------------- FCB ----------------------------------------------------------------------------- */

    t_fcb* crear_fcb_inexistente(char* nombre_archivo, t_filesystem* fs);
    void mostrar_info_fcb(t_fcb* fcb_a_mostrar, t_log* logger);
    void mostrar_bloques_fcb(t_list* bloques_fcb, t_log* logger, uint32_t puntero_directo);


/*------------------------------------------------------------------------- DIRECTORIO DEL FCB ----------------------------------------------------------------------------- */

    void levantar_fcbs_del_directorio(t_filesystem* fs, t_list* lista_fcbs_global);
    int el_directorio_fcb_esta_vacio(t_filesystem* fs);
    void crear_fcbs_del_directorio(t_filesystem* fs, t_list* lista_fcbs_global);
    void crear_directorios(t_filesystem* fs);


/*------------------------------------------------------------------------- OTRAS ----------------------------------------------------------------------------- */

    void cargar_t_filesystem(t_config* fs_config, t_config* superbloque_config, t_filesystem* fs);
    char* devolver_fcb_path_config(char* path, char* nombre_archivo);
    void cerrar_archivos();


#endif