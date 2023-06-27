#include "../include/fs-structures.h"

int fd_bitmap;
int fd_bloques;
void* map_bloques;
void* map ;
t_bitarray* bitmap;

/*------------------------------------------------------------------------- SUPERBLOQUE ----------------------------------------------------------------------------- */

void crear_superbloque_dat(t_filesystem* fs, t_config* superbloque) {

    int creado = config_save_in_file(superbloque, fs->superbloque_path);

    if (creado == -1) {
        log_error(fs->logger, "Error al crear el superbloque.dat");
    } else {
        log_info(fs->logger, "Superbloque creado correctamente");
    }

}

/*------------------------------------------------------------------------- BITMAP/BITARRAY ----------------------------------------------------------------------------- */

void levantar_bitmap(t_filesystem* fs) {

    FILE * bitmap_file = fopen(fs->bitmap_path, "rb");

    if (bitmap_file == NULL) {
        crear_bitmap(fs);
    } else {        
        abrir_bitmap(fs);
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

    uint32_t size_bitarray = ceil(fs->block_count / 8);

    if (ftruncate(fd_bitmap, size_bitarray) == -1) {
        log_info(fs->logger, "Error al setear el tamaño del bitmap. %s", strerror(errno));
        exit(1);
    }

    map = mmap(NULL, size_bitarray, PROT_WRITE | PROT_READ, MAP_SHARED, fd_bitmap, 0);
    if (map == MAP_FAILED) {
        log_error(fs->logger, "Fallo en el mmap del bitmap %s", strerror(errno));
        exit(1);
    } else {
        log_info(fs->logger, "Bitmap creado correctamente");
    }

    memset(map, 0, size_bitarray); // Seteo en cero a todo el map

    if (msync(map, size_bitarray, MS_SYNC) == -1) {
        log_info(fs->logger, "Error al sincronizar el bitmap. %s", strerror(errno));
        exit(1);
    }

    bitmap = bitarray_create(map, size_bitarray);

    for (int i = 0; i < fs->block_count; i++) {
        bitarray_clean_bit(bitmap, i); // LLENO EL BITARRAY CON CEROS
    }

}

void abrir_bitmap(t_filesystem* fs) {

    fd_bitmap = open(fs->bitmap_path, O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
    if (fd_bitmap == -1) {
        log_error(fs->logger, "Error al abrir el archivo de bitmap. %s", strerror(errno));
        exit(1);
    }

    uint32_t size_bitarray = ceil(fs->block_count / 8);

    map = mmap(NULL, size_bitarray, PROT_WRITE | PROT_READ, MAP_SHARED, fd_bitmap, 0);
    if (map == MAP_FAILED) {
        log_error(fs->logger, "Fallo en el mmap bitmap %s", strerror(errno));
        exit(1);
    } else {
        log_info(fs->logger, "Abrimos el bitmap ya creado");
    }

    bitmap = bitarray_create(map, size_bitarray);

}

/*------------------------------------------------------------------------- ARCHIVO DE BLOQUES ----------------------------------------------------------------------------- */

void levantar_archivo_de_bloques(t_filesystem* fs) {

    FILE * bloques_file = fopen(fs->bloques_path, "rb");

    if (bloques_file == NULL) {
        crear_archivo_de_bloques(fs);
    } else {
        abrir_archivo_de_bloques(fs);
        fclose(bloques_file);
    }

}

void crear_archivo_de_bloques(t_filesystem* fs) {

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

    map_bloques = mmap(NULL, size, PROT_WRITE | PROT_READ, MAP_SHARED, fd_bloques, 0);
    if (map_bloques == MAP_FAILED) {
        log_error(fs->logger, "Fallo en el mmap del archivo de bloques %s", strerror(errno));
        exit(1);
    } else {
        log_info(fs->logger, "Archivo de bloques creado correctamente");
    }

}

void abrir_archivo_de_bloques(t_filesystem* fs) {

    fd_bloques = open(fs->bloques_path, O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
    if (fd_bloques == -1) {
        log_error(fs->logger, "Error al abrir el archivo de bloques. %s", strerror(errno));
        exit(1);
    }

    uint32_t size = fs->block_count * fs->block_size;

    map_bloques = mmap(NULL, size, PROT_WRITE | PROT_READ, MAP_SHARED, fd_bloques, 0);
    if (map_bloques == MAP_FAILED) {
        log_error(fs->logger, "Fallo en el mmap del archivo de bloques. %s", strerror(errno));
        exit(1);
    } else {
        log_info(fs->logger, "Abrimos el archivo de bloques ya creado");
    }

}

uint32_t buscar_bloque_libre(t_filesystem* fs, uint32_t* bloque_filesystem) {

    uint32_t bloque_libre;
    uint32_t cant_bloques = fs->block_count / 8;

    for (uint32_t i = 0; i < cant_bloques; i++) {

        if (bitarray_test_bit(bitmap, i) != 1) { // O SEA SI ES CERO, SI EL BLOQUE ESTA LIBRE
            log_info(fs->logger, "\e[1;92mAcceso a Bitmap - Bloque: <%d> - Estado: <%d>\e[0m", i, bitarray_test_bit(bitmap, i));
            bloque_libre = i;
            *bloque_filesystem = i;
            bitarray_set_bit(bitmap, i);
            log_info(fs->logger, "\e[1;92mAcceso a Bitmap - Bloque: <%d> - Estado: <%d>\e[0m", i, bitarray_test_bit(bitmap, i));

            break;
        }

    }

    return bloque_libre;
}

void liberar_bloque(t_filesystem* fs, uint32_t* bloque_a_liberar) {

    log_info( fs->logger, "\e[1;92mAcceso a Bitmap - Bloque: <%ld> - Estado: <%d>\e[0m", (off_t)(*bloque_a_liberar), bitarray_test_bit( bitmap, (off_t)(*bloque_a_liberar) ) );
    bitarray_clean_bit(bitmap, (off_t)(*bloque_a_liberar));
    log_info( fs->logger, "\e[1;92mAcceso a Bitmap - Bloque: <%ld> - Estado: <%d>\e[0m", (off_t)(*bloque_a_liberar), bitarray_test_bit( bitmap, (off_t)(*bloque_a_liberar) ) );

}

/*------------------------------------------------------------------------- FCBS ----------------------------------------------------------------------------- */

t_fcb* crear_fcb(char* nombre_archivo, t_filesystem* fs) {

    DIR* fcbs_path;
    int creado = 0;
    struct dirent* directorio;
    t_fcb* fcb_nuevo = malloc(sizeof(t_fcb));

    // ABRO NUESTRA CARPETA QUE CONTIENE LOS FCBS.CONFIG

    fcbs_path = opendir("./fcbs");
    if (fcbs_path == NULL) {
        log_error(fs->logger, "Error al abrir el path propio de fcbs. %s", strerror(errno));
        exit(1);
    }

    // BUSCO EL CONFIG QUE TENGA EL NOMBRE DE ARCHIVO SOLICITADO Y CREO UN FCB CON SUS DATOS

    while( (directorio = readdir(fcbs_path)) ) {

        char *path_fcbs_config = string_new();
	    string_append(&path_fcbs_config, "./fcbs/");
	    string_append(&path_fcbs_config, directorio->d_name); // ------------------------------------------------------------------- VALGRIND MARCA ALGO EN ESTA LINEA                

        t_config* config_aux = config_create(path_fcbs_config); // ------------------------------------------------------------------- VALGRIND MARCA ALGO EN ESTA LINEA                
        if (config_aux == NULL) {
            log_info(fs->logger, "Esta fallando la creacion del config");
        }

        char* nombre_temporal = config_get_string_value(config_aux, "NOMBRE_ARCHIVO");

        if (strcmp(directorio->d_name, ".") == 0 || strcmp(directorio->d_name, "..") == 0) {
            continue;
        } else if (strcmp(nombre_temporal, nombre_archivo) == 0) {
            strcpy(fcb_nuevo->nombre_archivo, config_get_string_value(config_aux, "NOMBRE_ARCHIVO"));
            strcpy(fcb_nuevo->tamanio_archivo, config_get_string_value(config_aux, "TAMANIO_ARCHIVO"));
            fcb_nuevo->puntero_directo = config_get_int_value(config_aux, "PUNTERO_DIRECTO");
            fcb_nuevo->puntero_indirecto = config_get_int_value(config_aux, "PUNTERO_INDIRECTO");
            fcb_nuevo->bloques = list_create();
            fcb_nuevo->fcb_config = config_aux;

            crear_fcb_config_en_el_path(config_aux, fs, nombre_archivo);
            creado = 1;

            log_info(fs->logger, "Fue creado un FCB con nombre %s", fcb_nuevo->nombre_archivo);
        } else {
            config_destroy(config_aux);
        }
        
        free(path_fcbs_config);
    }

    if (!creado) {
        fcb_nuevo = NULL;
    }
    
    closedir(fcbs_path);    

    return fcb_nuevo;
}

t_fcb* crear_fcb_inexistente(char* nombre_archivo, t_filesystem* fs) {

    char* nuevo_path = string_new();
	string_append(&nuevo_path, "./fcbs/");
    string_append(&nuevo_path, nombre_archivo);
    string_append(&nuevo_path, ".config");

    FILE * fd = fopen(nuevo_path, "w+");
    if (fd == NULL) {
        log_info(fs->logger, "El path del nuevo config esta mal. %s", strerror(errno));
    }

    fprintf(fd, "NOMBRE_ARCHIVO=%s\n", nombre_archivo);
    fprintf(fd, "TAMANIO_ARCHIVO=%s\n", "0");
    fprintf(fd, "PUNTERO_DIRECTO=%u\n", 0);
    fprintf(fd, "PUNTERO_INDIRECTO=%u\n", 0);

    size_t longitud_nombre_archivo = strlen(nombre_archivo);
    size_t longitud_tamanio_archivo = strlen("0 ");
    t_fcb* fcb_nuevo = malloc(sizeof(t_fcb));

    fcb_nuevo->nombre_archivo = malloc( (longitud_nombre_archivo + 1) * sizeof(char) );
    strcpy(fcb_nuevo->nombre_archivo, nombre_archivo);
    fcb_nuevo->tamanio_archivo = malloc( (longitud_tamanio_archivo + 1) * sizeof(char) );
    strcpy(fcb_nuevo->tamanio_archivo, "0");
    fcb_nuevo->puntero_directo = 0;
    fcb_nuevo->puntero_indirecto = 0;    

    fclose(fd);

    t_config* config_nuevo_file = config_create(nuevo_path);
    fcb_nuevo->fcb_config = config_nuevo_file;
    fcb_nuevo->bloques = list_create();

    crear_fcb_config_en_el_path(config_nuevo_file, fs, nombre_archivo);

    return fcb_nuevo;
}

/*------------------------------------------------------------------------- CONFIG DEL FCB ----------------------------------------------------------------------------- */

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

void crear_directorios(t_filesystem* fs) {

    int resultado;

    resultado = mkdir("/home/utnso/fs", 0777);
    resultado = mkdir(fs->fcb_path, 0777);

    if (!resultado) {
        log_info(fs->logger, "Se crearon los directorios necesarios.");
    }

}

/*------------------------------------------------------------------------- OTRAS ----------------------------------------------------------------------------- */

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
    close(fd_bloques);
    close(fd_bitmap);
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
