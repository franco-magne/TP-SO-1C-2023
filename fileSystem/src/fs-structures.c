#include "../include/fs-structures.h"

t_superbloque* superbloque;

void load_bitmap(unsigned char** bitmap) {
    int fd = open("bitmap.dat", O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Error al abrir el archivo bitmap.dat\n");
        exit(1);
    }

    *bitmap = mmap(NULL, BITMAP_SIZE, PROT_READ, MAP_PRIVATE, fd, 0);
    if (*bitmap == MAP_FAILED) {
        fprintf(stderr, "Error al mapear el archivo bitmap.dat\n");
        exit(1);
    }

    close(fd);
}

void crear_estructuras_administrativas(t_filesystem* fs, t_log* fs_logger) {

    superbloque = malloc(sizeof(t_superbloque));
    FILE* fs_sb; 

    if ((fs_sb = fopen(fs->superbloque_path, "wb+")) == NULL) { // wb FICHERO BINARIO SI EXISTE O NO LO CREA; el + INDICA QUE ES LECTURA Y ESCRITURA
        log_error(fs_logger, "No se pudo abrir el archivo");
    }



}

void cargar_t_filesystem(t_config* config, t_config* sb_config, t_config* fcb_config, t_filesystem* fs) {

    fs->ip_memoria = config_get_string_value(config, "IP");
    fs->puerto_memoria = config_get_string_value(config,"PUERTO_MEMORIA");
    fs->puerto_escucha = config_get_string_value(config,"PUERTO_ESCUCHA");
    fs->superbloque_path = config_get_string_value(config, "PATH_SUPERBLOQUE");
    fs->bitmap_path = config_get_string_value(config, "PATH_BITMAP");
    fs->bloques_path = config_get_string_value(config, "PATH_BLOQUES");
    fs->fcb_path = config_get_string_value(config, "PATH_FCB");
    fs->retardo_accesos = config_get_double_value(config, "RETARDO_ACCESO_BLOQUE");

    fs->block_size = config_get_int_value(sb_config, "BLOCK_SIZE");
    fs->block_count = config_get_int_value(sb_config, "BLOCK_COUNT");

    fs->nombre_archivo = config_get_string_value(fcb_config, "NOMBRE_ARCHIVO");
    fs->tamanio_archivo = config_get_int_value(fcb_config, "TAMANIO_ARCHIVO");
    fs->puntero_directo = config_get_int_value(fcb_config, "PUNTERO_DIRECTO");
    fs->puntero_indirecto = config_get_int_value(fcb_config, "PUNTERO_INDIRECTO");

}
