#include "../include/fs-atender-kernel.h"

t_list* lista_fcbs;

/*------------------------------------------------------------------------- ATENDER KERNEL ----------------------------------------------------------------------------- */

void atender_kernel(t_filesystem* fs) {

    int operacion_OK = 0;
    lista_fcbs = list_create();
    levantar_fcbs_del_directorio(fs, lista_fcbs);

    // PARA PRUEBAS RAPIDAS
    
        crear_archivo("NotasSO", fs);
        abrir_archivo_fs("NotasSO", fs);
        truncar_archivo("NotasSO", 512, fs);        
        escribir_archivo("NotasSO", 4, 32, 2, fs);
    

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

                operacion_OK = abrir_archivo_fs(nombre_archivo_open, fs);
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
                    stream_send_empty_buffer(fs->socket_kernel, HANDSHAKE_ok_continue); // FALLO EN EL TRUNCATE
                }  

                free(nombre_archivo_truncate);
                buffer_destroy(bufferTruncate);

            break;

            case HEADER_f_read:

                char* nombre_archivo_read;
                uint32_t direccion_fisica_read;
                uint32_t cantidad_bytes_a_leer;
                uint32_t puntero_a_leer;
                t_buffer* bufferRead = buffer_create();

                stream_recv_buffer(fs->socket_kernel, bufferRead);
                nombre_archivo_read = buffer_unpack_string(bufferRead);
                buffer_unpack(bufferRead, &direccion_fisica_read, sizeof(direccion_fisica_read));
                buffer_unpack(bufferRead, &cantidad_bytes_a_leer, sizeof(cantidad_bytes_a_leer));
                buffer_unpack(bufferRead, &puntero_a_leer, sizeof(puntero_a_leer));

                log_info(fs->logger, "\e[1;93mRecibo operacion F_READ < %s, %d, %d> de KERNEL\e[0m", nombre_archivo_read, direccion_fisica_read, cantidad_bytes_a_leer);

                leer_archivo(nombre_archivo_read, direccion_fisica_read, cantidad_bytes_a_leer, puntero_a_leer, fs);
                stream_send_empty_buffer(fs->socket_kernel, HANDSHAKE_ok_continue); // NOTIFICO A KERNEL QUE YA SE COMPLETO LA LECTURA

                free(nombre_archivo_read);
                buffer_destroy(bufferRead);

            break;

            case HEADER_f_write:

                char* nombre_archivo_write;
                uint32_t direccion_logica_write;
                uint32_t cantidad_bytes_a_escribir;
                uint32_t puntero_a_escribir;
                t_buffer* bufferWrite = buffer_create();

                stream_recv_buffer(fs->socket_kernel, bufferWrite);
                nombre_archivo_write = buffer_unpack_string(bufferWrite);
                buffer_unpack(bufferWrite, &direccion_logica_write, sizeof(direccion_logica_write));
                buffer_unpack(bufferWrite, &cantidad_bytes_a_escribir, sizeof(cantidad_bytes_a_escribir));
                buffer_unpack(bufferWrite, &puntero_a_escribir, sizeof(puntero_a_escribir));

                log_info(fs->logger, "\e[1;93mRecibo operacion F_WRITE < %s, %d, %d> de KERNEL\e[0m", nombre_archivo_write, direccion_logica_write, cantidad_bytes_a_escribir);



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

int abrir_archivo_fs(char* nombre_archivo_open, t_filesystem* fs) {

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
            
            break;
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

/*------------------------------------------------------------------------- F_READ ----------------------------------------------------------------------------- */

int leer_archivo(char* nombre_archivo_read, uint32_t direccion_fisica, uint32_t cant_bytes_a_leer, uint32_t puntero_proceso, t_filesystem* fs) {

    int cant_bloques_a_leer = (int)ceil(cant_bytes_a_leer / fs->block_size);
    int posicion_fcb_a_leer= devolver_posicion_fcb_en_la_lista(nombre_archivo_read);
    t_fcb* fcb_a_leer = list_get(lista_fcbs, posicion_fcb_a_leer);

    char* cadena_final =  malloc(cant_bytes_a_leer + 1);
    
    if (cant_bloques_a_leer > 1) {
        
        uint32_t bytes_en_array[cant_bloques_a_leer];
        convertir_cantidad_bytes_en_array(cant_bytes_a_leer, bytes_en_array, fs->block_size);

        for (int i = 0; i < cant_bloques_a_leer; i++) {

            char* cadena_aux = malloc(bytes_en_array[i]);
            uint32_t* bloque_lectura = (uint32_t*)list_get( fcb_a_leer->bloques, (puntero_proceso + 1 + i) ); // LE SUMO UNO PORQUE EN LA POSICION CERO ESTA EL PUNTERO INDIRECTO
            
            log_info(fs->logger, "\e[1;92mAcceso Bloque - Archivo: <%s> - Bloque Archivo: <%d> - Bloque File System <%d>\e[0m", nombre_archivo_read, (puntero_proceso + 1 + i), (*bloque_lectura));
            strcpy(cadena_aux, leer_puntero_del_archivo_de_bloques((*bloque_lectura), bytes_en_array[i], fs));
            string_append(&cadena_final, cadena_aux);

            intervalo_de_pausa(fs->retardo_accesos);
            free(cadena_aux);
        }

        // REALIZAR ACA EL ENVIO DE CADENA_FINAL A MEMORIA Y ESPERAR SU RESPUESTA.


        
    } else {

        uint32_t* bloque_lectura = (uint32_t*)list_get( fcb_a_leer->bloques, (puntero_proceso + 1) ); // LE SUMO UNO PORQUE EN LA POSICION CERO ESTA EL PUNTERO INDIRECTO

        log_info(fs->logger, "\e[1;92mAcceso Bloque - Archivo: <%s> - Bloque Archivo: <%d> - Bloque File System <%d>\e[0m", nombre_archivo_read, (puntero_proceso + 1), (*bloque_lectura));
        strcpy(cadena_final, leer_puntero_del_archivo_de_bloques((*bloque_lectura), cant_bytes_a_leer, fs));
        
        intervalo_de_pausa(fs->retardo_accesos);

        // REALIZAR ACA EL ENVIO DE CADENA_FINAL A MEMORIA Y ESPERAR SU RESPUESTA.



    }

    log_info(fs->logger, "Leer Archivo: <%s> - Puntero: <%d> - Memoria: <%d> - Tamaño: <%d>", nombre_archivo_read, puntero_proceso, direccion_fisica, cant_bytes_a_leer);

    return 1;
}

/*------------------------------------------------------------------------- F_WRITE ----------------------------------------------------------------------------- */

int escribir_archivo(char* nombre_archivo_write, uint32_t direccion_fisica, uint32_t cant_bytes_a_escribir, uint32_t puntero_proceso, t_filesystem* fs) {

    char* respuesta_memoria = malloc(cant_bytes_a_escribir);
    int cant_bloques_a_escribir = (int)ceil(cant_bytes_a_escribir / fs->block_size);
    int posicion_fcb_a_escribir= devolver_posicion_fcb_en_la_lista(nombre_archivo_write);
    t_fcb* fcb_a_escribir = list_get(lista_fcbs, posicion_fcb_a_escribir);

    pedir_informacion_a_memoria(direccion_fisica, cant_bytes_a_escribir, fs, &respuesta_memoria);

    if (cant_bloques_a_escribir > 1) {
        
        uint32_t bytes_en_array[cant_bloques_a_escribir];
        char** cadena_array = convertir_cadena_caracteres_en_array(respuesta_memoria, cant_bytes_a_escribir, fs->block_size);

        convertir_cantidad_bytes_en_array(cant_bytes_a_escribir, bytes_en_array, fs->block_size);

        for (int i = 0; i < cant_bloques_a_escribir; i++) {
            
            uint32_t* bloque_escritura = (uint32_t*)list_get( fcb_a_escribir->bloques, (puntero_proceso + 1 + i) ); // LE SUMO UNO PORQUE EN LA POSICION CERO ESTA EL PUNTERO INDIRECTO
            log_info(fs->logger, "\e[1;92mAcceso Bloque - Archivo: <%s> - Bloque Archivo: <%d> - Bloque File System <%d>\e[0m", nombre_archivo_write, (puntero_proceso + 1 + i), (*bloque_escritura));
            escribir_en_puntero_del_archivo_de_bloques((*bloque_escritura), bytes_en_array[i], cadena_array[i], fs);

            intervalo_de_pausa(fs->retardo_accesos);
        }

        liberar_memoria_array_caracteres(cadena_array);

    } else {

        uint32_t* bloque_escritura = (uint32_t*)list_get( fcb_a_escribir->bloques, (puntero_proceso + 1) ); // LE SUMO UNO PORQUE EN LA POSICION CERO ESTA EL PUNTERO INDIRECTO

        log_info(fs->logger, "\e[1;92mAcceso Bloque - Archivo: <%s> - Bloque Archivo: <%d> - Bloque File System <%d>\e[0m", nombre_archivo_write, (puntero_proceso + 1), (*bloque_escritura));
        escribir_en_puntero_del_archivo_de_bloques((*bloque_escritura), cant_bytes_a_escribir, respuesta_memoria, fs);

        intervalo_de_pausa(fs->retardo_accesos);
    }

    log_info(fs->logger, "Escribir Archivo: <%s> - Puntero: <%d> - Memoria: <%d> - Tamaño: <%d>", nombre_archivo_write, puntero_proceso, direccion_fisica, cant_bytes_a_escribir);

    free(respuesta_memoria);

    return 1;
}

void pedir_informacion_a_memoria(uint32_t direccion_fisica, uint32_t cant_bytes_necesarios, t_filesystem* fs, char** respuesta_memoria) {

    t_buffer* bufferMemoria = buffer_create();
    t_buffer* buffer_respuesta_memoria = buffer_create();


    // ENVIO A MEMORIA LA DIRECCION FISICA Y CANTIDAD DE BYTES

    buffer_pack(bufferMemoria, &direccion_fisica, sizeof(direccion_fisica));
    buffer_pack(bufferMemoria, &cant_bytes_necesarios, sizeof(cant_bytes_necesarios));
    stream_send_buffer(fs->socket_memoria, HEADER_f_write, bufferMemoria);    
    log_info(fs->logger, "Se envia la direccion y cantidad de bytes a MEMORIA");


    // ESPERO LA RESPUESTA DE MEMORIA CON LO LEIDO
    
    log_info(fs->logger, "Esperando respuesta de MEMORIA...");
    stream_recv_buffer(fs->socket_memoria, buffer_respuesta_memoria);
    *respuesta_memoria = buffer_unpack_string(buffer_respuesta_memoria);
    log_info(fs->logger, "Recibo de MEMORIA: %s", (*respuesta_memoria));

    buffer_destroy(buffer_respuesta_memoria);
    buffer_destroy(bufferMemoria);    
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

/*------------------------------------------------------------------------- OTRAS ----------------------------------------------------------------------------- */

int devolver_posicion_fcb_en_la_lista(char* nombre_archivo) {

    int posicion_fcb;
    int size_lista_fcbs = list_size(lista_fcbs);

    for (int i = 0; i < size_lista_fcbs; i++) {

        t_fcb* fcb_aux = list_get(lista_fcbs, i);
        if (strcmp(fcb_aux->nombre_archivo, nombre_archivo) == 0) {            
            posicion_fcb = i;

            break;
        }
    }  

    return posicion_fcb;
}

void convertir_cantidad_bytes_en_array(uint32_t cantidad_bytes, uint32_t* array_bytes, uint32_t block_size) {
    
    uint32_t bytes_restantes = cantidad_bytes % block_size;
    int cantidad_block_size_repetido = cantidad_bytes / block_size;

    for (int i = 0; i < cantidad_block_size_repetido; i++) {
        array_bytes[i] = 64;
    }

    array_bytes[cantidad_block_size_repetido] = bytes_restantes; // LA POSICION FINAL GUARDO EL RESTO DE BYTES QUE QUEDAN
}

char** convertir_cadena_caracteres_en_array(char* cadena_recibida, uint32_t cantidad_bytes, uint32_t block_size) {
    
    uint32_t division_entera = cantidad_bytes / block_size;
    uint32_t resto = cantidad_bytes % block_size;
    int tamano_resultado = division_entera + (resto > 0 ? 1 : 0);

    char** array = malloc(tamano_resultado * sizeof(char*));

    for (uint32_t i = 0; i < tamano_resultado; i++) {
        array[i] = malloc((block_size + 1) * sizeof(char));
        strncpy(array[i], cadena_recibida + i * block_size, block_size);
        array[i][block_size] = '\0';
    }

    if (resto != 0) {
        array[division_entera] = malloc((resto + 1) * sizeof(char));
        strncpy(array[division_entera], cadena_recibida + division_entera * block_size, resto);
        array[division_entera][resto] = '\0';
    }

    return array;
}

void liberar_memoria_array_caracteres(char** array_caracteres) {

    int longitud = obtener_longitud_array_caracteres(array_caracteres);

    for (int i = 0; i < longitud; i++) {
        free(array_caracteres[i]);
    }

    free(array_caracteres);
}

int obtener_longitud_array_caracteres(char** array_caracteres) {
    int longitud = 0;

    while (array_caracteres[longitud] != NULL) {
        longitud++;
    }

    return longitud;
}
