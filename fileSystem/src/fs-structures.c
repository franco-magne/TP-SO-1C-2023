#include "../include/fs-structures.h"

void cargar_t_filesystem(t_config* config, t_filesystem* fs) {

    fs->ip_memoria = config_get_string_value(config, "IP");
    fs->puerto_memoria = config_get_string_value(config,"PUERTO_MEMORIA");
    fs->puerto_escucha = config_get_string_value(config,"PUERTO_ESCUCHA");
    fs->superbloque_path = config_get_string_value(config, "PATH_SUPERBLOQUE");
    fs->bitmap_path = config_get_string_value(config, "PATH_BITMAP");
    fs->bloques_path = config_get_string_value(config, "PATH_BLOQUES");
    fs->fcb_path = config_get_string_value(config, "PATH_FCB");
    fs->retardo_accesos = config_get_double_value(config, "RETARDO_ACCESO_BLOQUE");

}