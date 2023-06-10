#include "../include/fs-structures.h"

t_bitarray* bitmap;

void levantar_bitmap(t_filesystem* fs, t_log* logger) {

    int fd = open(fs->bitmap_path, O_RDONLY); 

    if (fd == -1) {
        log_error(logger, "Error al abrir el archivo de bitmap.");
        exit(1);
    }

    off_t valor = lseek(fd, fs->block_count - 1, SEEK_SET); // Me aseguro que el fd tenga -block_count- bytes. Me desplazo

    if (valor == -1) {
        log_error(logger, "Error al desplazarse por el archivo.");
        exit(1);
    }

    uint32_t size_bitarray = ceil(fs->block_count / 8); // Redondea hacia arriba
    void* map = mmap(NULL, fs->block_count, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);

    if (map == MAP_FAILED) {
        log_error(logger, "Fallo en el mmap.");
        exit(1);
    }

    bitmap = bitarray_create(map, size_bitarray);

    // void* blq = mmap();
    // memcpy([bloque], offset: nro bloque + tamanio);

}

void crear_estructuras_administrativas(t_filesystem* fs, t_log* fs_logger) {

    // EL SUPERBLOQUE SOLO ES EL ARCHIVO CONFIG. NO HAY ESTRUCTURAS.
    // MAS DATA: EL BITMAP SE CORRESPONDE CON EL ARCHIVO DE BLOQUES Y ES UNO SOLO BITMAP PARA TODO EL FYLESYSTEM.
    // LOS ARCHIVOS QUE ENTREN A FYLESYSTEM ENTRAN COMO .CONFIG

    // CREACION DEL ARCHIVO DE BLOQUES

    int fd = open(fs->bloques_path, O_RDONLY); 
    if (fd == -1) {
        log_error(fs_logger, "Error al abrir el archivo de bloques.");
        exit(1);
    }

    off_t valor = lseek(fd, (fs->block_count * fs->block_size) - 1, SEEK_SET); // Me aseguro que el fd tenga -block_count * block_size- bytes. Me desplazo
    if (valor == -1) {
        log_error(fs_logger, "Error al desplazarse por el archivo.");
        exit(1);
    }

    uint32_t size = fs->block_count * fs->block_size;
    void* map_bloques = mmap(NULL, size, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0); // map_bloques REEMPLAZA AL ARRAY DE BLOQUES

    if (map_bloques == MAP_FAILED) {
        log_error(fs_logger, "Fallo en el mmap.");
        exit(1);
    }

    // LA APERTURA DE ARCHIVOS (FCBs) CONVIENE HACERLA "A DEMANDA"

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

    fs->block_size = config_get_int_value(sb_config, "BLOCK_SIZE"); // DEL SUPERBLOQUE
    fs->block_count = config_get_int_value(sb_config, "BLOCK_COUNT"); // DEL SUPERBLOQUE

    fs->nombre_archivo = config_get_string_value(fcb_config, "NOMBRE_ARCHIVO"); // DEL FCB
    fs->tamanio_archivo = config_get_int_value(fcb_config, "TAMANIO_ARCHIVO"); // DEL FCB
    fs->puntero_directo = config_get_int_value(fcb_config, "PUNTERO_DIRECTO"); // DEL FCB
    fs->puntero_indirecto = config_get_int_value(fcb_config, "PUNTERO_INDIRECTO"); // DEL FCB

}
