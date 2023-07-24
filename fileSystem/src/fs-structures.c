#include "../include/fs-structures.h"

int fd_bitmap;
int fd_bloques;
void* map_bitmap;
void* map_bloques;
t_bitarray* bitmap;

/*------------------------------------------------------------------------- SUPERBLOQUE ----------------------------------------------------------------------------- */

void crear_superbloque_dat(t_filesystem* fs, t_config* superbloque) {

    int creado = config_save_in_file(superbloque, fs->superbloque_path);

    if (creado == -1) {
        log_error(fs->logger, "Error al crear el superbloque.dat. %s", strerror(errno));
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

    map_bitmap = mmap(NULL, size_bitarray, PROT_WRITE | PROT_READ, MAP_SHARED, fd_bitmap, 0);
    if (map_bitmap == MAP_FAILED) {
        log_error(fs->logger, "Fallo en el mmap del bitmap %s", strerror(errno));
        exit(1);
    } else {
        log_info(fs->logger, "Bitmap creado correctamente");
    }

    memset(map_bitmap, 0, size_bitarray); // Seteo en cero a todo el map

    if (msync(map_bitmap, size_bitarray, MS_SYNC) == -1) {
        log_info(fs->logger, "Error al sincronizar el bitmap. %s", strerror(errno));
        exit(1);
    }

    bitmap = bitarray_create(map_bitmap, size_bitarray);

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

    map_bitmap = mmap(NULL, size_bitarray, PROT_WRITE | PROT_READ, MAP_SHARED, fd_bitmap, 0);
    if (map_bitmap == MAP_FAILED) {
        log_error(fs->logger, "Fallo en el mmap bitmap. %s", strerror(errno));
        exit(1);
    } else {
        log_info(fs->logger, "Abrimos el bitmap ya creado");
    }

    bitmap = bitarray_create(map_bitmap, size_bitarray);
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

void buscar_bloque_libre(t_filesystem* fs, uint32_t* bloque_libre) {

    uint32_t cant_bloques = fs->block_count / 8;

    for (uint32_t i = 0; i < cant_bloques; i++) {

        log_info(fs->logger, GREEN BOLD "Acceso a Bitmap - Bloque: <%" PRIu32 "> - Estado: <%d>", i, bitarray_test_bit(bitmap, i));

        if (bitarray_test_bit(bitmap, i) != 1) { // O SEA SI ES CERO, SI EL BLOQUE ESTA LIBRE
            
            *bloque_libre = i;
            bitarray_set_bit(bitmap, i);
            log_info(fs->logger, GREEN BOLD "Acceso a Bitmap - Bloque: <%" PRIu32 "> - Estado: <%d>", i, bitarray_test_bit(bitmap, i));

            break;
        }
    }
}

void liberar_bloque(t_filesystem* fs, uint32_t* bloque_a_liberar) {

    log_info( fs->logger, GREEN BOLD "Acceso a Bitmap - Bloque: <%ld> - Estado: <%d>", (off_t)(*bloque_a_liberar), bitarray_test_bit( bitmap, (off_t)(*bloque_a_liberar) ) );
    bitarray_clean_bit(bitmap, (off_t)(*bloque_a_liberar));
    log_info( fs->logger, GREEN BOLD "Acceso a Bitmap - Bloque: <%ld> - Estado: <%d>", (off_t)(*bloque_a_liberar), bitarray_test_bit( bitmap, (off_t)(*bloque_a_liberar) ) );

}

void escribir_bloque_de_punteros_en_puntero_indirecto(uint32_t puntero_indirecto, uint32_t numero_puntero_X, uint32_t* puntero_a_escribir, uint32_t block_size) {    

    if (numero_puntero_X != 0) {

        uint32_t offset = (numero_puntero_X - 1) * sizeof(uint32_t);
        uint32_t posicion_puntero_indirecto_en_bytes = puntero_indirecto * block_size + offset;
        void* posicion_byte_a_escribir = map_bloques + posicion_puntero_indirecto_en_bytes;

        memcpy(posicion_byte_a_escribir, puntero_a_escribir, sizeof(uint32_t));
    }   
}

void liberar_puntero_del_bloque_de_punteros_en_puntero_indirecto(uint32_t puntero_indirecto, uint32_t posicion_ultimo_puntero, uint32_t block_size) {

    uint32_t offset = (posicion_ultimo_puntero - 1) * sizeof(uint32_t);
    uint32_t posicion_puntero_a_liberar_en_bytes = puntero_indirecto * block_size + offset;
    void* posicion_en_bytes_a_liberar = map_bloques + posicion_puntero_a_liberar_en_bytes;

    memset(posicion_en_bytes_a_liberar, 0, sizeof(uint32_t));
}

char* leer_puntero_del_archivo_de_bloques(uint32_t puntero_acceder, uint32_t bytes_a_leer, t_filesystem* fs) {

    char* cadena = malloc(bytes_a_leer + 1);
    uint32_t posicion_puntero_a_leer_en_bytes = puntero_acceder * fs->block_size;

    if ( bitarray_test_bit(bitmap, puntero_acceder) == 1 ) {

        log_info(fs->logger, GREEN BOLD "Acceso a Bitmap - Bloque: <%" PRIu32 "> - Estado: <%d>", puntero_acceder, 1);
        memcpy(cadena, map_bloques + posicion_puntero_a_leer_en_bytes, bytes_a_leer);
    }

    cadena[bytes_a_leer] = '\0'; // DELIMITADOR PARA EVITAR DATOS INCONCISOS AL ENVIAR EL BUFFER STRING

    return cadena;
}

void escribir_en_puntero_del_archivo_de_bloques(uint32_t puntero_acceder, uint32_t bytes_a_escribir, char* cadena_a_escribir, t_filesystem* fs) {

    uint32_t posicion_byte_a_escribir = puntero_acceder * fs->block_size;

    if ( bitarray_test_bit(bitmap, puntero_acceder) == 1 ) {

        log_info(fs->logger, GREEN BOLD "Acceso a Bitmap - Bloque: <%" PRIu32 "> - Estado: <%d>", puntero_acceder, 1);
        memcpy(map_bloques + posicion_byte_a_escribir, cadena_a_escribir, bytes_a_escribir);
    }
}

t_list* recuperar_bloque_de_punteros(uint32_t puntero_indirecto, int tamanio_archivo, uint32_t block_size) {
    
    t_list* lista_bloques = list_create();
    int cant_bloques_a_recuperar = (tamanio_archivo / block_size) - 2; // MENOS DOS PORQUE RESTO EL PUNTERO DIRECTO E INDIRECTO QUE YA TIENEN

    if ( bitarray_test_bit(bitmap, puntero_indirecto) == 1 ) {
        
        uint32_t* bloque_auxiliar = malloc(sizeof(uint32_t));
        *bloque_auxiliar = puntero_indirecto;

        list_add(lista_bloques, bloque_auxiliar);
    }

    int posicion_bitmap_a_acceder = puntero_indirecto + 1; // MAS UNO PORQUE QUIERO A PARTIR DEL SIGUIENTE BLOQUE DEL PUNTERO INDIRECTO
    for (int i = 0; i <= cant_bloques_a_recuperar; i++) {

        if ( bitarray_test_bit(bitmap, posicion_bitmap_a_acceder) == 1 ) {

            uint32_t offset = i * sizeof(uint32_t);
            uint32_t posicion_puntero_indirecto_en_bytes = puntero_indirecto * block_size + offset;
            void* posicion_byte_a_obtener = map_bloques + posicion_puntero_indirecto_en_bytes;
            uint32_t* bloques_restantes = malloc(sizeof(uint32_t));

            memcpy(bloques_restantes, posicion_byte_a_obtener, sizeof(uint32_t));

            list_add(lista_bloques, bloques_restantes);
            posicion_bitmap_a_acceder++;
        }
        
    }

    return lista_bloques;
}

/*------------------------------------------------------------------------- FCB ----------------------------------------------------------------------------- */

t_fcb* crear_fcb_inexistente(char* nombre_archivo, t_filesystem* fs) {

    char* nuevo_path = devolver_fcb_path_config(fs->fcb_path, nombre_archivo);
    string_append(&nuevo_path, ".config");

    FILE * fd = fopen(nuevo_path, "w+");
    if (fd == NULL) {
        log_info(fs->logger, "Error en el path del directorio FCBs. %s", strerror(errno));
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

    t_config* config_nuevo_file = config_create(nuevo_path);
    fcb_nuevo->fcb_config = config_nuevo_file;
    fcb_nuevo->bloques = list_create();

    free(nuevo_path);
    fclose(fd);

    return fcb_nuevo;
}

void mostrar_info_fcb(t_fcb* fcb_a_mostrar, t_log* logger) {

    log_info(logger, " ---> Nombre: %s", fcb_a_mostrar->nombre_archivo);
    log_info(logger, " ---> Tamanio: %s", fcb_a_mostrar->tamanio_archivo);
    log_info(logger, " ---> Puntero directo: %" PRIu32, fcb_a_mostrar->puntero_directo);
    log_info(logger, " ---> Puntero indirecto: %" PRIu32 , fcb_a_mostrar->puntero_indirecto);    
}

void mostrar_bloques_fcb(t_list* bloques, t_log* logger, uint32_t puntero_directo) {

    int size_bloques = list_size(bloques);

    if (size_bloques > 0) {
        char* cadena_bloques = malloc((size_bloques * 12) + 1); // Tamaño máximo por bloque: 10 dígitos + 2 espacios + 1 terminador null
        snprintf(cadena_bloques, 12, "%u", puntero_directo);
        strcat(cadena_bloques, "  ");

        for (int i = 1; i < size_bloques; i++) {
            uint32_t* bloque = (uint32_t*)list_get(bloques, i);
            char bloque_str[12]; // Suficiente para almacenar un uint32_t como cadena

            snprintf(bloque_str, sizeof(bloque_str), "%u", *bloque);
            strcat(cadena_bloques, bloque_str);
            strcat(cadena_bloques, "  ");
        }

        log_info(logger, " ---> Cant. bloques: %d", size_bloques);
        log_info(logger, " ---> Bloques de datos: %s", cadena_bloques);

        free(cadena_bloques);

    } else if (puntero_directo >= 0 && size_bloques == 0) {

        char* puntero_directo_str = string_itoa(puntero_directo);

        log_info(logger, " ---> Bloques de datos: %s", puntero_directo_str);

        free(puntero_directo_str);

    } else {

        log_info(logger, " ---> Cant. bloques: %d", size_bloques);
    }
}

/*------------------------------------------------------------------------- DIRECTORIO DEL FCB ----------------------------------------------------------------------------- */

void levantar_fcbs_del_directorio(t_filesystem* fs, t_list* lista_fcbs) {

    if (el_directorio_fcb_esta_vacio(fs)) {
        log_info(fs->logger, "No hay FCBs en el directorio para levantar");
    } else {
        log_info(fs->logger, "Procedemos a levantar los FCBs del directorio...");
        crear_fcbs_del_directorio(fs, lista_fcbs);
    }

}

int el_directorio_fcb_esta_vacio(t_filesystem* fs) {

    DIR* dir = opendir(fs->fcb_path);
    if (dir == NULL) {
        log_info(fs->logger, "No se pudo abrir el directorio. %s", strerror(errno));
        exit(1);
    }

    int contador = 0;
    struct dirent* entrada;
    
    while ( (entrada = readdir(dir)) != NULL ) {
        // IGNORO LAS ENTRADAS "." Y ".."
        if (strcmp(entrada->d_name, ".") == 0 || strcmp(entrada->d_name, "..") == 0) {
            continue;
        }
        contador++;
    }

    closedir(dir);

    return (contador == 0) ? 1 : 0;
}

void crear_fcbs_del_directorio(t_filesystem* fs, t_list* lista_fcbs) {

    int contador = 1;
    DIR* directorio_fcbs;
    struct dirent* entrada;
    
    // ABRO EL DIRECTORIO DE FCBs

    directorio_fcbs = opendir(fs->fcb_path);
    if (directorio_fcbs == NULL) {
        log_error(fs->logger, "Error al abrir el directorio de FCBs. %s", strerror(errno));
        exit(1);
    }

    // LEO LOS ARCHIVOS DEL DIRECTORIO Y CREO LOS FCBs A MEMORIA    

    while( (entrada = readdir(directorio_fcbs)) ) {               

        if (strcmp(entrada->d_name, ".") == 0 || strcmp(entrada->d_name, "..") == 0) {
            continue;
        } else {

            t_fcb* fcb_existente = malloc(sizeof(t_fcb));
            char* path_fcb_config = devolver_fcb_path_config(fs->fcb_path, entrada->d_name);  

            t_config* config_aux = config_create(path_fcb_config);
            if (config_aux == NULL) {
                log_info(fs->logger, "Esta fallando la creacion del config. %s", strerror(errno));
            }

            size_t longitud_nombre_archivo = strlen( config_get_string_value(config_aux, "NOMBRE_ARCHIVO") );
            size_t longitud_tamanio_archivo = strlen( config_get_string_value(config_aux, "TAMANIO_ARCHIVO") );

            fcb_existente->nombre_archivo = malloc( (longitud_nombre_archivo + 1) * sizeof(char) );
            strcpy( fcb_existente->nombre_archivo, config_get_string_value(config_aux, "NOMBRE_ARCHIVO") );
            fcb_existente->tamanio_archivo = malloc( (longitud_tamanio_archivo + 1) * sizeof(char) );
            strcpy( fcb_existente->tamanio_archivo, config_get_string_value(config_aux, "TAMANIO_ARCHIVO") );
            fcb_existente->puntero_directo = config_get_int_value(config_aux, "PUNTERO_DIRECTO");
            fcb_existente->puntero_indirecto = config_get_int_value(config_aux, "PUNTERO_INDIRECTO");
            fcb_existente->fcb_config = config_aux;
            
            if ( atoi(fcb_existente->tamanio_archivo) > fs->block_size ) {
                fcb_existente->bloques = recuperar_bloque_de_punteros(fcb_existente->puntero_indirecto, atoi(fcb_existente->tamanio_archivo), fs->block_size);
            } else {
                fcb_existente->bloques = list_create();
            }                                

            log_info(fs->logger, "%d) FCB levantado del directorio", contador);
            mostrar_info_fcb(fcb_existente, fs->logger);
            mostrar_bloques_fcb(fcb_existente->bloques, fs->logger, fcb_existente->puntero_directo);

            free(path_fcb_config);
            contador++;

            list_add(lista_fcbs, fcb_existente);
        }         
    }    

    log_info(fs->logger, "Cantidad de FCBs levantados del directorio: %d", list_size(lista_fcbs));

    closedir(directorio_fcbs);
}

void crear_directorios(t_filesystem* fs) {

    int resultado;

    resultado = mkdir("./fs", 0777);      // RUTA ANTERIOR: /home/utnso/fs   -   RUTA PARA LA ENTREGA FINAL: ./fs
    resultado = mkdir(fs->fcb_path, 0777);

    if (!resultado) {
        log_info(fs->logger, "Directorios creados");
    }
}

/*------------------------------------------------------------------------- OTRAS ----------------------------------------------------------------------------- */

void cargar_t_filesystem(t_config* config, t_config* sb_config, t_filesystem* fs) {

    fs->ip_escucha = config_get_string_value(config, "IP_ESCUCHA");
    fs->ip_memoria = config_get_string_value(config, "IP_MEMORIA");
    fs->puerto_memoria = config_get_string_value(config,"PUERTO_MEMORIA");
    fs->puerto_escucha = config_get_string_value(config,"PUERTO_ESCUCHA");
    fs->superbloque_path = config_get_string_value(config, "PATH_SUPERBLOQUE");
    fs->bitmap_path = config_get_string_value(config, "PATH_BITMAP");
    fs->bloques_path = config_get_string_value(config, "PATH_BLOQUES");
    fs->fcb_path = config_get_string_value(config, "PATH_FCB");
    fs->retardo_accesos = (uint32_t) config_get_int_value(config, "RETARDO_ACCESO_BLOQUE");

    fs->block_size = config_get_int_value(sb_config, "BLOCK_SIZE"); // DEL SUPERBLOQUE
    fs->block_count = config_get_int_value(sb_config, "BLOCK_COUNT"); // DEL SUPERBLOQUE
}

char* devolver_fcb_path_config(char* path_fcbs, char* nombre_archivo) {
	
    char* path_fcbs_config = string_new();
    string_append(&path_fcbs_config, path_fcbs);
    string_append(&path_fcbs_config, "/");
    string_append(&path_fcbs_config, nombre_archivo);

	return path_fcbs_config;
}

void cerrar_archivos() {
    close(fd_bloques);
    close(fd_bitmap);
}