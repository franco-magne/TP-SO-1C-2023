#include "../include/fs-atender-kernel.h"

t_list* lista_fcbs;

/*------------------------------------------------------------------------- ATENDER KERNEL ----------------------------------------------------------------------------- */

void atender_kernel(t_filesystem* fs) {

    int operacion_OK = 0;
    lista_fcbs = list_create();
    levantar_fcbs_del_directorio(fs, lista_fcbs);
    
    // PARA PRUEBAS RAPIDAS
    /* 
        // 1) F_CREATE
        crear_archivo("NotasSO", fs);
        crear_archivo("NotasDDS", fs);
        crear_archivo("NotasGDD", fs);

        // 2) F_OPEN
        abrir_archivo("NotasSO", fs);
        abrir_archivo("NotasDDS", fs);
        abrir_archivo("NotasGDD", fs);

        // 3) F_TRUNCATE
        abrir_archivo("NotasSO", fs);
        sleep(2);
        truncar_archivo("NotasSO", 512, fs);
        sleep(3);
        abrir_archivo("NotasDDS", fs);
        sleep(2);
        truncar_archivo("NotasDDS", 384, fs);
        sleep(3);
        truncar_archivo("NotasSO", 256, fs);

        F_OPEN NotasSO
        F_OPEN NotasDDS
        F_TRUNCATE NotasSO 512
        F_TRUNCATE NotasDDS 512
        F_TRUNCATE NotasSO 256
        F_OPEN NotasGDD
        F_TRUNCATE NotasGDD 512
        EXIT

    */

   // Bloques de 64 bytes pueden contener 16 punteros. y si necesita mas un archivo?
   // Preguntar sobre los logs si estan bien y el retardo de acceso. va en truncate?
   // log sobre bitmap

    while (fs->socket_kernel != -1) {
        
        log_info(fs->logger, "Esperando peticion de KERNEL...");
        uint8_t header = stream_recv_header(fs->socket_kernel); // RECIBO LA OPERACION QUE KERNEL QUIERA SOLICITAR        

        switch(header) {

            case HEADER_f_open:                
                                        
                char* nombre_archivo_open;
                t_buffer* bufferOpen = buffer_create();                

                stream_recv_buffer(fs->socket_kernel, bufferOpen); // RECIBO EL BUFFER NOMBRE DE ARCHIVO DE KERNEL
                nombre_archivo_open = buffer_unpack_string(bufferOpen); // DESERIALIZO EL BUFFER MANDADO POR KERNEL

                log_info(fs->logger, "\e[1;93mRecibo operacion F_OPEN <%s> de KERNEL\e[0m", nombre_archivo_open);

                operacion_OK = abrir_archivo(nombre_archivo_open, fs);
                if (operacion_OK) {
                    stream_send_empty_buffer(fs->socket_kernel, HANDSHAKE_ok_continue); // NOTIFICO A KERNEL QUE EL ARCHIVO EXISTE Y LO AGREGUE A SU TABLA GLOBAL
                } else {
                    log_info(fs->logger, "El archivo no existe. Solicite crearlo"); // NO EXISTE ESE ARCHIVO. TIENE QUE SOLICITAR CREARLO PARA AGREGARLO AL DIRECTORIO DE FCBs
                    stream_send_empty_buffer(fs->socket_kernel, HEADER_f_create);                
                }

                free(nombre_archivo_open);
                buffer_destroy(bufferOpen);

            break;

            case HEADER_f_create:

                char* nombre_archivo_create;
                t_buffer* bufferCreate = buffer_create();

                stream_recv_buffer(fs->socket_kernel, bufferCreate); // RECIBO EL BUFFER NOMBRE DE ARCHIVO DE KERNEL
                nombre_archivo_create = buffer_unpack_string(bufferCreate); // DESERIALIZO EL BUFFER MANDADO POR KERNEL

                log_info(fs->logger, "\e[1;93mRecibo operacion F_CREATE <%s> de KERNEL\e[0m", nombre_archivo_create);

                operacion_OK = crear_archivo(nombre_archivo_create, fs);
                if (operacion_OK) {
                    stream_send_empty_buffer(fs->socket_kernel, HANDSHAKE_ok_continue);
                } else {
                    log_info(fs->logger, "Error al crear el nuevo archivo.");
                    stream_send_empty_buffer(fs->socket_kernel, HEADER_error);
                }

                free(nombre_archivo_create);
                buffer_destroy(bufferCreate);

            break;

            case HEADER_f_truncate:                

                char* nombre_archivo_truncate;
                uint32_t tamanio_archivo_truncate;
                t_buffer* bufferTruncate = buffer_create();                

                stream_recv_buffer(fs->socket_kernel, bufferTruncate); // RECIBO EL BUFFER NOMBRE DE ARCHIVO DE KERNEL
                nombre_archivo_truncate = buffer_unpack_string(bufferTruncate); // DESERIALIZO EL BUFFER MANDADO POR KERNEL
                buffer_unpack(bufferTruncate, &tamanio_archivo_truncate, sizeof(tamanio_archivo_truncate)); // DESERIALIZO EL TAMANIO DE ARCHIVO

                log_info(fs->logger, "\e[1;93mRecibo operacion F_TRUNCATE <%s, %d> de KERNEL\e[0m", nombre_archivo_truncate, tamanio_archivo_truncate);

                operacion_OK = truncar_archivo(nombre_archivo_truncate, tamanio_archivo_truncate, fs);
                if (operacion_OK) {                    
                    stream_send_empty_buffer(fs->socket_kernel, HANDSHAKE_ok_continue); // NOTIFICO A KERNEL QUE EL ARCHIVO SE TRUNCO
                } else {
                    log_info(fs->logger, "Error al truncar. No puede pedir mas bloques");
                    stream_send_empty_buffer(fs->socket_kernel, HEADER_error); // FALLO EN EL TRUNCATE
                }  

                free(nombre_archivo_truncate);
                buffer_destroy(bufferTruncate);

            break;

            case HEADER_f_read:

                char* nombre_archivo_read;
                uint32_t direccion_logica_read;
                uint32_t cantidad_bytes_a_leer;
                t_buffer* bufferRead = buffer_create();

                stream_recv_buffer(fs->socket_kernel, bufferRead);
                nombre_archivo_read = buffer_unpack_string(bufferRead);
                buffer_unpack(bufferRead, &direccion_logica_read, sizeof(direccion_logica_read));
                buffer_unpack(bufferRead, &cantidad_bytes_a_leer, sizeof(cantidad_bytes_a_leer));

                log_info(fs->logger, "\e[1;93mRecibo operacion F_READ < , , > de KERNEL\e[0m");



                free(nombre_archivo_read);
                buffer_destroy(bufferRead);

            break;

            case HEADER_f_write:

                char* nombre_archivo_write;
                uint32_t direccion_logica_write;
                uint32_t cantidad_bytes_a_escribir;
                t_buffer* bufferWrite = buffer_create();

                stream_recv_buffer(fs->socket_kernel, bufferWrite);
                nombre_archivo_write = buffer_unpack_string(bufferWrite);
                buffer_unpack(bufferWrite, &direccion_logica_write, sizeof(direccion_logica_write));
                buffer_unpack(bufferWrite, &cantidad_bytes_a_escribir, sizeof(cantidad_bytes_a_escribir));

                log_info(fs->logger, "\e[1;93mRecibo operacion F_WRITE < , , > de KERNEL\e[0m");



                free(nombre_archivo_write);
                buffer_destroy(bufferWrite);

            break;

            default:
                log_error(fs->logger, "Peticion incorrecta.");
                exit(1);
            break;
        }

        operacion_OK = 0;
    }
    
    printf("Hubo una desconexion");
    return;
}

/*------------------------------------------------------------------------- F_OPEN ----------------------------------------------------------------------------- */

int abrir_archivo(char* nombre_archivo_open, t_filesystem* fs) {

    int encontrado = 0;
    int size_lista_fcbs = list_size(lista_fcbs);
    
    for (int i = 0; i < size_lista_fcbs; i++) {

        t_fcb* fcb_aux = list_get(lista_fcbs, i);

        if (strcmp(fcb_aux->nombre_archivo, nombre_archivo_open) == 0) {

            log_info(fs->logger, "\e[1;92mAbrir archivo: <%s>\e[0m", nombre_archivo_open);
            log_info(fs->logger, "Datos del FCB:");
            mostrar_info_fcb(fcb_aux, fs->logger);
            mostrar_bloques_fcb(fcb_aux->bloques, fs->logger, fcb_aux->puntero_directo);
            log_info(fs->logger, "Archivo <%s> abierto correctamente", nombre_archivo_open);

            encontrado = 1;
        }
    }

    return encontrado;
}

/*------------------------------------------------------------------------- F_CREATE ----------------------------------------------------------------------------- */

int crear_archivo(char* nombre_archivo_create, t_filesystem* fs) {

    int resultado = 0;
    t_fcb* fcb_nuevo = crear_fcb_inexistente(nombre_archivo_create, fs);

    if (fcb_nuevo != NULL) {
    
        log_info(fs->logger, "\e[1;92mCrear archivo: <%s>\e[0m", nombre_archivo_create);
        log_info(fs->logger, "Datos del FCB:");
        mostrar_info_fcb(fcb_nuevo, fs->logger);
        mostrar_bloques_fcb(fcb_nuevo->bloques, fs->logger, fcb_nuevo->puntero_directo);
        log_info(fs->logger, "Archivo <%s> creado y abierto correctamente", nombre_archivo_create);

        list_add(lista_fcbs, fcb_nuevo);
        resultado = 1;        
    }

    return resultado;
}

/*------------------------------------------------------------------------- F_TRUNCATE ----------------------------------------------------------------------------- */

int truncar_archivo(char* nombre_archivo_truncate, uint32_t nuevo_tamanio_archivo, t_filesystem* fs) {
        
    t_fcb* fcb_truncado;
    int truncado_ok = 0;
    int pos_archivo_a_truncar;
    int respuesta_afirmativa = 0;
    uint32_t fcb_a_truncar_tamanio_actual;
    int size_lista_fcbs = list_size(lista_fcbs);
    
    // BUSCO LA POSICION EN LA QUE SE ENCUENTRA EL FCB A TRUNCAR DENTRO DE LA LISTA DE FCBs
    for (int i = 0; i < size_lista_fcbs; i++) {

        t_fcb* fcb_aux = list_get(lista_fcbs, i);
        if (strcmp(fcb_aux->nombre_archivo, nombre_archivo_truncate) == 0) {            
            fcb_a_truncar_tamanio_actual = atoi(fcb_aux->tamanio_archivo);
            
            if (nuevo_tamanio_archivo > fcb_a_truncar_tamanio_actual) {
                respuesta_afirmativa = puede_ampliar_tamanio(fcb_aux, fs->block_size, nuevo_tamanio_archivo, fcb_a_truncar_tamanio_actual);
            }

            pos_archivo_a_truncar = i;
        }
    }    

    // PROCEDO A HACER EL TRUNCATE         
    if ( nuevo_tamanio_archivo < fcb_a_truncar_tamanio_actual ) {

        fcb_truncado = reducir_tamanio_archivo(nombre_archivo_truncate, nuevo_tamanio_archivo, fs, pos_archivo_a_truncar);
        truncado_ok = 1;
        
    } else if ( nuevo_tamanio_archivo > fcb_a_truncar_tamanio_actual && respuesta_afirmativa ) {

        fcb_truncado = ampliar_tamanio_archivo(nombre_archivo_truncate, nuevo_tamanio_archivo, fs, pos_archivo_a_truncar);
        truncado_ok = 1;
    }

    if (truncado_ok) {

        log_info(fs->logger, "Salimos del bitmap y el archivo de bloques");
        log_info(fs->logger, "\e[1;92mTruncar Archivo: <%s> - Tamaño: <%d>\e[0m", nombre_archivo_truncate, nuevo_tamanio_archivo);
        log_info(fs->logger, "FCB DESPUES: ");
        mostrar_info_fcb(fcb_truncado, fs->logger);
        mostrar_bloques_fcb(fcb_truncado->bloques, fs->logger, fcb_truncado->puntero_directo);
        log_info(fs->logger, "Archivo <%s, %d> truncado correctamente", nombre_archivo_truncate, nuevo_tamanio_archivo); 
    }           

    return truncado_ok;
}

t_fcb* ampliar_tamanio_archivo(char* nombre_archivo_truncate, uint32_t nuevo_tamanio_archivo, t_filesystem* fs, int pos_archivo_a_ampliar) {

    t_fcb* fcb_a_truncar = list_get(lista_fcbs, pos_archivo_a_ampliar);
    uint32_t fcb_a_truncar_tamanio = atoi(fcb_a_truncar->tamanio_archivo);

    char* nuevo_tamanio_en_char = string_itoa(nuevo_tamanio_archivo);
    size_t longitud_nueva = strlen( nuevo_tamanio_en_char );

    log_info(fs->logger, "Truncate resulta en AMPLIAR. Tamanio actual es menor al nuevo solicitado");
    log_info(fs->logger, "FCB ANTES: ");
    mostrar_info_fcb(fcb_a_truncar, fs->logger);
    mostrar_bloques_fcb(fcb_a_truncar->bloques, fs->logger, fcb_a_truncar->puntero_directo);
    log_info(fs->logger, "Accedemos al bitmap y al archivo de bloques");

    uint32_t bloque_libre;
    uint32_t cant_bloques_necesarios = (nuevo_tamanio_archivo - fcb_a_truncar_tamanio) / fs->block_size;

    // PUNTERO DIRECTO
    if (list_size(fcb_a_truncar->bloques) == 0) {

        buscar_bloque_libre(fs, &bloque_libre);
        fcb_a_truncar->puntero_directo = bloque_libre;
        cant_bloques_necesarios--;

        log_info(fs->logger, "\e[1;92mAcceso Bloque - Archivo: <%s> - Bloque Archivo: <1> - Bloque File System <%d>\e[0m", nombre_archivo_truncate, bloque_libre);
    }

    // PUNTERO INDIRECTO Y BLOQUE DE PUNTEROS
    for (uint32_t i = 0; i < cant_bloques_necesarios; i++) {
        
        buscar_bloque_libre(fs, &bloque_libre);
        if (fcb_a_truncar->puntero_indirecto == 0) {
            fcb_a_truncar->puntero_indirecto = bloque_libre;
        }

        uint32_t* nuevo_bloque = malloc(sizeof(uint32_t)); // IMPORTANTE: PARA NO APUNTAR SIEMPRE AL MISMO PUNTERO -- VALGRIND: 16 BYTES PERDIDOS
        *nuevo_bloque = bloque_libre; // IMPORTANTE: PARA NO APUNTAR SIEMPRE AL MISMO PUNTERO
        list_add(fcb_a_truncar->bloques, nuevo_bloque);
        
        uint32_t puntero_numero_X = list_size(fcb_a_truncar->bloques) - 1; // RESTO UNO PORQUE NO TENGO QUE COPIAR EL PUNTERO INDIRECTO EN EL ARCHIVO DE BLOQUES
        escribir_bloque_de_punteros_en_puntero_indirecto(fcb_a_truncar->puntero_indirecto, puntero_numero_X, nuevo_bloque, fs->block_size);

        log_info(fs->logger, "\e[1;92mAcceso Bloque - Archivo: <%s> - Bloque Archivo: <%d> - Bloque File System <%d>\e[0m", nombre_archivo_truncate, (list_size(fcb_a_truncar->bloques) + 1), bloque_libre);
    }

    char* retorno_ampliar = realloc(fcb_a_truncar->tamanio_archivo, (longitud_nueva + 1) * sizeof(char));
    if (retorno_ampliar == NULL) {
        log_error(fs->logger, "Error en el realloc. %s", strerror(errno));
    } else {
        fcb_a_truncar->tamanio_archivo = retorno_ampliar;
        strcpy(fcb_a_truncar->tamanio_archivo, nuevo_tamanio_en_char );
    }
    
    config_set_value(fcb_a_truncar->fcb_config, "NOMBRE_ARCHIVO", fcb_a_truncar->nombre_archivo);
    config_set_value(fcb_a_truncar->fcb_config, "TAMANIO_ARCHIVO", fcb_a_truncar->tamanio_archivo);
    config_set_value( fcb_a_truncar->fcb_config, "PUNTERO_DIRECTO", string_itoa(fcb_a_truncar->puntero_directo) ); // VALGRIND: 7 BYTES PERDIDOS
    config_set_value( fcb_a_truncar->fcb_config, "PUNTERO_INDIRECTO", string_itoa(fcb_a_truncar->puntero_indirecto) ); // VALGRIND: 7 BYTES PERDIDOS
    config_save(fcb_a_truncar->fcb_config);

    free(nuevo_tamanio_en_char);

    return fcb_a_truncar;
}

int puede_ampliar_tamanio(t_fcb* fcb_a_ampliar, uint32_t block_size, uint32_t nuevo_tamanio, uint32_t fcb_tamanio_actual) {

    int cantidad_bloques_fcb = list_size(fcb_a_ampliar->bloques) - 1;
    int cantidad_maxima_bloques = block_size / sizeof(uint32_t);
    int cantidad_bloques_necesarios = (nuevo_tamanio - fcb_tamanio_actual) / block_size;

    if (cantidad_bloques_fcb + cantidad_bloques_necesarios > cantidad_maxima_bloques) {
        return 0;
    } else {
        return 1;
    }

}

t_fcb* reducir_tamanio_archivo(char* nombre_archivo_truncate, uint32_t nuevo_tamanio_archivo, t_filesystem* fs, int pos_archivo_a_reducir) {

    t_fcb* fcb_a_truncar = list_get(lista_fcbs, pos_archivo_a_reducir);
    uint32_t fcb_a_truncar_tamanio = atoi(fcb_a_truncar->tamanio_archivo);

    char* nuevo_tamanio_en_char = string_itoa(nuevo_tamanio_archivo);
    size_t longitud_nueva = strlen( nuevo_tamanio_en_char );

    log_info(fs->logger, "Truncate resulta en REDUCIR. Tamanio actual es mayor al nuevo solicitado");
    log_info(fs->logger, "FCB ANTES: ");
    mostrar_info_fcb(fcb_a_truncar, fs->logger);
    mostrar_bloques_fcb(fcb_a_truncar->bloques, fs->logger, fcb_a_truncar->puntero_directo);
    log_info(fs->logger, "Accedemos al bitmap y al archivo de bloques");

    int ultima_posicion_lista_bloques;
    uint32_t cant_bloques_a_liberar = (fcb_a_truncar_tamanio - nuevo_tamanio_archivo) / fs->block_size;
    
    if (nuevo_tamanio_archivo == 0) {
        cant_bloques_a_liberar -= 1;
    }

    for (uint32_t i = 0; i < cant_bloques_a_liberar; i++) {

        ultima_posicion_lista_bloques = list_size(fcb_a_truncar->bloques) - 1;
        uint32_t* bloque_a_liberar = (uint32_t*)list_get(fcb_a_truncar->bloques, ultima_posicion_lista_bloques);

        liberar_bloque(fs, bloque_a_liberar);
        list_remove(fcb_a_truncar->bloques, ultima_posicion_lista_bloques);
        liberar_puntero_del_bloque_de_punteros_en_puntero_indirecto(fcb_a_truncar->puntero_indirecto, ultima_posicion_lista_bloques, fs->block_size);

        log_info(fs->logger, "\e[1;92mAcceso Bloque - Archivo: <%s> - Bloque Archivo: <%d> - Bloque File System <%d>\e[0m", nombre_archivo_truncate, (ultima_posicion_lista_bloques + 1), (int)(*bloque_a_liberar));
    }            

    char* retorno_reducir = realloc(fcb_a_truncar->tamanio_archivo, (longitud_nueva + 1) * sizeof(char));      
    if (retorno_reducir == NULL) {
        log_error(fs->logger, "Error en el realloc. %s", strerror(errno));
    } else {
        fcb_a_truncar->tamanio_archivo = retorno_reducir;
        strcpy(fcb_a_truncar->tamanio_archivo, nuevo_tamanio_en_char );
    }

    if (nuevo_tamanio_archivo == fs->block_size) {
        fcb_a_truncar->puntero_indirecto = 0;
    }

    if (nuevo_tamanio_archivo == 0) {
        liberar_bloque(fs, &fcb_a_truncar->puntero_directo);
        fcb_a_truncar->puntero_directo = 0;
        fcb_a_truncar->puntero_indirecto = 0;
    }

    config_set_value(fcb_a_truncar->fcb_config, "NOMBRE_ARCHIVO", fcb_a_truncar->nombre_archivo);
    config_set_value(fcb_a_truncar->fcb_config, "TAMANIO_ARCHIVO", fcb_a_truncar->tamanio_archivo);            
    config_set_value( fcb_a_truncar->fcb_config, "PUNTERO_DIRECTO", string_itoa(fcb_a_truncar->puntero_directo) ); // VALGRIND: 2 BYTES PERDIDOS
    config_set_value( fcb_a_truncar->fcb_config, "PUNTERO_INDIRECTO", string_itoa(fcb_a_truncar->puntero_indirecto) ); // VALGRIND: 2 BYTES PERDIDOS
    config_save(fcb_a_truncar->fcb_config);

    free(nuevo_tamanio_en_char);

    return fcb_a_truncar;
}

int leer_archivo(char* nombre_archivo, uint32_t direccion_logica, uint32_t cant_bytes_a_leer) {



    return 1;
}

int escribir_archivo(char* nombre_archivo, uint32_t direccion_logica, uint32_t cant_bytes_a_escribir) {


    return 1;
}

/*------------------------------------------------------------------------- ESPERAR KERNEL ----------------------------------------------------------------------------- */

int fs_escuchando_en(int server_fs, t_filesystem* fs) {

    pthread_t hilo;
    int socket_kernel = esperar_cliente(server_fs);
    
    fs->socket_kernel = socket_kernel;
    log_info(fs->logger, "Cliente KERNEL conectado");

    if (socket_kernel != -1) {
    
        pthread_create(&hilo, NULL, (void*) atender_kernel, (void*)fs);
        pthread_join(hilo, NULL);
        
        return 1;
    }

    return 0;
}
