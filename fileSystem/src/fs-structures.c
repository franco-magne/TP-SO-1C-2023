#include "../include/fs-structures.h"

t_bitarray* bitmap;
int fd_bitmap;
int fd_bloques;
void* map_bloques;

void levantar_bitmap(t_filesystem* fs) {

    fd_bitmap = open(fs->bitmap_path, O_RDONLY); 
    if (fd_bitmap == -1) {
        log_error(fs->logger, "Error al abrir el archivo de bitmap.");
        exit(1);
    }

    off_t valor = lseek(fd_bitmap, fs->block_count - 1, SEEK_SET); // Me aseguro que el fd tenga -block_count- bytes. Me desplazo
    if (valor == -1) {
        log_error(fs->logger, "Error al desplazarse por el archivo.");
        exit(1);
    }

    uint32_t size_bitarray = ceil(fs->block_count / 8); // Redondea hacia arriba
    void* map = mmap(NULL, fs->block_count, PROT_WRITE | PROT_READ, MAP_SHARED, fd_bitmap, 0);

    if (map == MAP_FAILED) {
        log_error(fs->logger, "Fallo en el mmap.");
        exit(1);
    }

    bitmap = bitarray_create(map, size_bitarray);

    // void* blq = mmap();
    // memcpy([bloque], offset: nro bloque + tamanio);

}

void crear_archivo_de_bloques(t_filesystem* fs) {

    // EL SUPERBLOQUE SOLO ES EL ARCHIVO CONFIG. NO HAY ESTRUCTURAS.
    // MAS DATA: EL BITMAP SE CORRESPONDE CON EL ARCHIVO DE BLOQUES Y ES UNO SOLO BITMAP PARA TODO EL FYLESYSTEM.
    // LOS ARCHIVOS QUE ENTREN A FYLESYSTEM ENTRAN COMO .CONFIG

    // CREACION DEL ARCHIVO DE BLOQUES

    fd_bloques = open(fs->bloques_path, O_RDONLY); 
    if (fd_bloques == -1) {
        log_error(fs->logger, "Error al abrir el archivo de bloques.");
        exit(1);
    }

    off_t valor = lseek(fd_bloques, (fs->block_count * fs->block_size) - 1, SEEK_SET); // Me aseguro que el fd tenga -block_count * block_size- bytes. Me desplazo
    if (valor == -1) {
        log_error(fs->logger, "Error al desplazarse por el archivo.");
        exit(1);
    }

    uint32_t size = fs->block_count * fs->block_size;
    map_bloques = mmap(NULL, size, PROT_WRITE | PROT_READ, MAP_SHARED, fd_bloques, 0); // map_bloques REEMPLAZA AL ARRAY DE BLOQUES

    if (map_bloques == MAP_FAILED) {
        log_error(fs->logger, "Fallo en el mmap.");
        exit(1);
    }

}

t_list* crear_fcbs(t_log* fs_logger) {

    DIR* fcbs_path;
    struct dirent* directorio;
    t_list* lista = malloc(sizeof(t_list));

    // ABRO LA CARPETA QUE CONTIENE LOS FCBS.CONFIG

    fcbs_path = opendir("../fcbs");
    if (fcbs_path == NULL) {
        log_error(fs_logger, "Error al abrir el path de fcbs.");
        exit(1);
    }

    // CREO UNA LISTA DE STRUCTS FCBS DE LOS ARCHIVOS .CONFIG

    while((directorio = readdir(fcbs_path))) {
        if (strstr(directorio->d_name, ".config")) { // PARA SABER SI EL ARCHIVO CONTIENE .CONFIG
            
            t_fcb* fcb_aux = malloc(sizeof(t_fcb));
            t_config* config_aux = config_create(strcat("../fcbs/", directorio->d_name));
            
            fcb_aux->nombre_archivo = config_get_string_value(config_aux, "NOMBRE_ARCHIVO");
            fcb_aux->tamanio_archivo = config_get_int_value(config_aux, "TAMANIO_ARCHIVO");
            fcb_aux->puntero_directo = config_get_int_value(config_aux, "PUNTERO_DIRECTO");
            fcb_aux->puntero_indirecto = config_get_int_value(config_aux, "PUNTERO_INDIRECTO");

            list_add(lista, fcb_aux);
            config_destroy(config_aux);
            free(fcb_aux);
        }
    }

    return lista;
}

void cargar_t_filesystem(t_config* config, t_config* sb_config, t_filesystem* fs) {

    fs->ip_memoria = config_get_string_value(config, "IP_MEMORIA");
    fs->puerto_memoria = config_get_string_value(config,"PUERTO_MEMORIA");
    fs->puerto_escucha = config_get_string_value(config,"PUERTO_ESCUCHA");
    fs->superbloque_path = config_get_string_value(config, "PATH_SUPERBLOQUE");
    fs->bitmap_path = config_get_string_value(config, "PATH_BITMAP");
    fs->bloques_path = config_get_string_value(config, "PATH_BLOQUES");
    fs->fcb_path = config_get_string_value(config, "PATH_FCB");
    fs->retardo_accesos = config_get_double_value(config, "RETARDO_ACCESO_BLOQUE");

    fs->block_size = config_get_int_value(sb_config, "BLOCK_SIZE"); // DEL SUPERBLOQUE
    fs->block_count = config_get_int_value(sb_config, "BLOCK_COUNT"); // DEL SUPERBLOQUE

}

void cerrar_archivos() {
    close(fd_bitmap);
    close(fd_bloques);
}
