#include "../include/fs-structures.h"

int fd_bitmap;
int fd_bloques;
void* map_bloques;
t_bitarray* bitmap;

void crear_superbloque_dat(t_filesystem* fs, t_config* superbloque) {

    int creado = config_save_in_file(superbloque, fs->superbloque_path);

    if (creado == -1) {
        log_error(fs->logger, "Error al crear el superbloque.dat");
    } else {
        log_info(fs->logger, "Tenemos superbloque.dat ok.");
    }

}

void levantar_bitmap(t_filesystem* fs) {

    FILE * bitmap_file = fopen(fs->bitmap_path, "rb");

    if (bitmap_file == NULL) {
        crear_bitmap(fs);
    } else {
        log_info(fs->logger, "Levantamos el bitmap ya creado.");
        fclose(bitmap_file);
    }

    // void* blq = mmap();
    // memcpy([bloque], offset: nro bloque + tamanio);    
}

void crear_bitmap(t_filesystem* fs) {

    fd_bitmap = open(fs->bitmap_path, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
    if (fd_bitmap == -1) {
        log_error(fs->logger, "Error al abrir el archivo de bitmap. %s", strerror(errno));
        exit(1);
    }

    uint32_t size_bitarray = ceil(fs->block_count / 8); // Redondea hacia arriba

    if (ftruncate(fd_bitmap, size_bitarray) == -1) {
        log_info(fs->logger, "Error al setear el tamaño del bitmap. %s", strerror(errno));
        exit(1);
    }

    void* map = mmap(NULL, size_bitarray, PROT_WRITE | PROT_READ, MAP_SHARED, fd_bitmap, 0); // iba fs.block_count en el segundo parametro
    if (map == MAP_FAILED) {
        log_error(fs->logger, "Fallo en el mmap. %s", strerror(errno));
        exit(1);
    } else {
        log_info(fs->logger, "Tenemos bitmap ok.");
    }

    memset(map, 0, size_bitarray); // Seteo en cero a todo el map

    if (msync(map, size_bitarray, MS_SYNC) == -1) {
        log_info(fs->logger, "Error al sincronizar el bitmap. %s", strerror(errno));
        exit(1);
    }

    bitmap = bitarray_create(map, size_bitarray);

}

void levantar_archivo_de_bloques(t_filesystem* fs) {

    FILE * bloques_file = fopen(fs->bloques_path, "rb");

    if (bloques_file == NULL) {
        crear_archivo_de_bloques(fs);
    } else {
        log_info(fs->logger, "Levantamos el archivo de bloques ya creado.");
        fclose(bloques_file);
    }

}

void crear_archivo_de_bloques(t_filesystem* fs) {

    // MAS DATA: EL BITMAP SE CORRESPONDE CON EL ARCHIVO DE BLOQUES Y ES UN SOLO BITMAP PARA TODO EL FYLESYSTEM.
    // LOS ARCHIVOS QUE ENTREN A FYLESYSTEM ENTRAN COMO .CONFIG

    fd_bloques = open(fs->bloques_path, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO); 
    if (fd_bloques == -1) {
        log_error(fs->logger, "Error al abrir el archivo de bloques. %s", strerror(errno));
        exit(1);
    }

    uint32_t size = fs->block_count * fs->block_size;

    if (ftruncate(fd_bloques, size) == -1) {
        log_info(fs->logger, "Error al setear el tamaño del archivo de bloques. %s", strerror(errno));
        exit(1);
    }

    map_bloques = mmap(NULL, size, PROT_WRITE | PROT_READ, MAP_SHARED, fd_bloques, 0); // map_bloques REEMPLAZA AL ARRAY DE BLOQUES
    if (map_bloques == MAP_FAILED) {
        log_error(fs->logger, "Fallo en el mmap. %s", strerror(errno));
        exit(1);
    } else {
        log_info(fs->logger, "Tenemos archivo de bloques ok.");
    }

}

t_fcb* crear_fcb(char* nombre_archivo, t_filesystem* fs) {

    DIR* fcbs_path;
    int creado = 0;
    struct dirent* directorio;
    t_fcb* fcb_nuevo = malloc(sizeof(t_fcb));

    // ABRO NUESTRA CARPETA QUE CONTIENE LOS FCBS.CONFIG

    fcbs_path = opendir("./fcbs");
    if (fcbs_path == NULL) {
        log_error(fs->logger, "Error al abrir el path de fcbs. %s", strerror(errno));
        exit(1);
    } else {
        log_info(fs->logger, "FILESYSTEM accedio al directorio propio de FCBs.");
    }

    // BUSCO EL CONFIG QUE TENGA EL NOMBRE DE ARCHIVO SOLICITADO Y CREO UN FCB CON SUS DATOS

    while( (directorio = readdir(fcbs_path)) ) {

        char* nombre_temporal = string_new();
        char *path_fcbs_config = string_new();
	    string_append(&path_fcbs_config, "./fcbs/");
	    string_append(&path_fcbs_config, directorio->d_name);

        t_config* config_aux = config_create(path_fcbs_config);

        if (config_aux == NULL) {
            log_info(fs->logger, "NO existe esa ruta");
        }

        nombre_temporal = config_get_string_value(config_aux, "NOMBRE_ARCHIVO");

        if (strcmp(directorio->d_name, ".") == 0 || strcmp(directorio->d_name, "..") == 0) {
            continue;
        } else if (strcmp(nombre_temporal, nombre_archivo) == 0) {
            fcb_nuevo->nombre_archivo = config_get_string_value(config_aux, "NOMBRE_ARCHIVO");
            fcb_nuevo->tamanio_archivo = config_get_string_value(config_aux, "TAMANIO_ARCHIVO");
            fcb_nuevo->puntero_directo = config_get_int_value(config_aux, "PUNTERO_DIRECTO");
            fcb_nuevo->puntero_indirecto = config_get_int_value(config_aux, "PUNTERO_INDIRECTO");
            fcb_nuevo->fcb_config = config_aux;

            crear_fcb_config_en_el_path(config_aux, fs, nombre_archivo);
            creado = 1;

            log_info(fs->logger, "Fue creado un FCB con nombre %s", fcb_nuevo->nombre_archivo);
        } else {
            config_destroy(config_aux);
        }

        free(nombre_temporal);
        free(path_fcbs_config);
    }

    if (!creado) {
        fcb_nuevo = NULL;
    }
    
    closedir(fcbs_path);    

    return fcb_nuevo;
}

void crear_fcb_config_en_el_path(t_config* config_fcb, t_filesystem* fs, char* nombre_archivo) {

    char* final_path = string_new();
	string_append(&final_path, fs->fcb_path);
	string_append(&final_path, "/");
    string_append(&final_path, nombre_archivo);
    string_append(&final_path, ".config");

    int creado = config_save_in_file(config_fcb, final_path);

    if (creado == -1) {
        log_error(fs->logger, "Error al crear el config");
    } else {
        log_info(fs->logger, "Se creo el config en %s", final_path);
    }

    free(final_path);
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

void crear_directorios(t_filesystem* fs) {

    int resultado;

    resultado = mkdir("/home/utnso/fs", 0777);
    resultado = mkdir(fs->fcb_path, 0777);

    if (!resultado) {
        log_info(fs->logger, "Se crearon los directorios necesarios.");
    }

}

void cerrar_archivos() {
    close(fd_bitmap);
    close(fd_bloques);
}

char* concatenar(char* str1, char* str2, t_filesystem* fs) {
	char* new_str;
	if ((new_str = malloc(strlen(str1) + strlen(str2) + 1)) != NULL) {
		new_str[0] = '\0';
		strcat(new_str, str1);
		strcat(new_str, str2);
	} else {
		log_error(fs->logger, "Error al concatenar");
	}

	return new_str;
}
