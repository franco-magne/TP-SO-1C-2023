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
    */

    while (fs->socket_kernel != -1) {
        
        log_info(fs->logger, "Esperando peticion de KERNEL...");
        uint8_t header = stream_recv_header(fs->socket_kernel); // RECIBO LA OPERACION QUE KERNEL QUIERA SOLICITAR        

        switch(header) {

            case HEADER_f_open:                
                                        
                char* nombre_archivo_open;
                t_buffer* buffer_nombre_archivo_open = buffer_create();                

                stream_recv_buffer(fs->socket_kernel, buffer_nombre_archivo_open); // RECIBO EL BUFFER NOMBRE DE ARCHIVO DE KERNEL
                nombre_archivo_open = buffer_unpack_string(buffer_nombre_archivo_open); // DESERIALIZO EL BUFFER MANDADO POR KERNEL

                log_info(fs->logger, "\e[1;93mRecibo operacion F_OPEN <%s> de KERNEL\e[0m", nombre_archivo_open);

                operacion_OK = abrir_archivo(nombre_archivo_open, fs);
                if (operacion_OK) {
                    stream_send_empty_buffer(fs->socket_kernel, HANDSHAKE_ok_continue); // NOTIFICO A KERNEL QUE EL ARCHIVO EXISTE Y LO AGREGUE A SU TABLA GLOBAL
                } else {
                    log_info(fs->logger, "El archivo no existe. Solicite crearlo"); // NO EXISTE ESE ARCHIVO. TIENE QUE SOLICITAR CREARLO PARA AGREGARLO AL DIRECTORIO DE FCBs
                    stream_send_empty_buffer(fs->socket_kernel, HEADER_f_create);                
                }

                free(nombre_archivo_open);
                buffer_destroy(buffer_nombre_archivo_open);

            break;

            case HEADER_f_create:

                char* nombre_archivo_create;
                t_buffer* buffer_nombre_archivo_create = buffer_create();

                stream_recv_buffer(fs->socket_kernel, buffer_nombre_archivo_create); // RECIBO EL BUFFER NOMBRE DE ARCHIVO DE KERNEL
                nombre_archivo_create = buffer_unpack_string(buffer_nombre_archivo_create); // DESERIALIZO EL BUFFER MANDADO POR KERNEL

                log_info(fs->logger, "\e[1;93mRecibo operacion F_CREATE <%s> de KERNEL\e[0m", nombre_archivo_create);

                operacion_OK = crear_archivo(nombre_archivo_create, fs);
                if (operacion_OK) {
                    stream_send_empty_buffer(fs->socket_kernel, HANDSHAKE_ok_continue);
                } else {
                    log_info(fs->logger, "Error al crear el nuevo archivo.");
                    stream_send_empty_buffer(fs->socket_kernel, HEADER_error);
                }

                free(nombre_archivo_create);
                buffer_destroy(buffer_nombre_archivo_create);

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
                    log_info(fs->logger, "Error al truncar. Algo paso.");
                    stream_send_empty_buffer(fs->socket_kernel, HEADER_error); // FALLO EN EL TRUNCATE
                }  

                free(nombre_archivo_truncate);
                buffer_destroy(bufferTruncate);

            break;

            case HEADER_f_read:

                log_info(fs->logger, "\e[1;93mRecibo operacion F_READ < , , > de KERNEL\e[0m");



            break;

            case HEADER_f_write:

                log_info(fs->logger, "\e[1;93mRecibo operacion F_WRITE < , , > de KERNEL\e[0m");

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
            mostrar_bloques_fcb(fcb_aux->bloques, fs->logger);
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
        mostrar_bloques_fcb(fcb_nuevo->bloques, fs->logger);
        log_info(fs->logger, "Archivo <%s> creado correctamente", nombre_archivo_create);

        list_add(lista_fcbs, fcb_nuevo); // Preguntar. Con esta linea estoy asumiendo que despues de crear el archivo tambien se esta abriendo
        resultado = 1;        
    }

    return resultado;
}

/*------------------------------------------------------------------------- F_TRUNCATE ----------------------------------------------------------------------------- */

int truncar_archivo(char* nombre_archivo_truncate, uint32_t nuevo_tamanio_archivo, t_filesystem* fs) {

    int truncado_ok;
    int pos_archivo_a_truncar = -1;
    int size_lista_fcbs = list_size(lista_fcbs);
    
    // BUSCO LA POSICION EN LA QUE SE ENCUENTRA EL FCB A TRUNCAR DENTRO DE LA LISTA DE FCBs

    for (int i = 0; i < size_lista_fcbs; i++) {

        t_fcb* fcb_aux = list_get(lista_fcbs, i);

        if (strcmp(fcb_aux->nombre_archivo, nombre_archivo_truncate) == 0) {
            pos_archivo_a_truncar = i;
        }
    }

    // PROCEDO A HACER EL TRUNCATE

    if (pos_archivo_a_truncar == -1) {
        truncado_ok = 0; // NO SE ENCONTRO UN ARCHIVO CON ESE NOMBRE
    } else {        
        
        t_fcb* fcb_a_truncar = list_get(lista_fcbs, pos_archivo_a_truncar);
        uint32_t fcb_a_truncar_tamanio = atoi(fcb_a_truncar->tamanio_archivo);
        char* nuevo_tamanio_en_char = string_itoa(nuevo_tamanio_archivo);
        size_t longitud_nueva = strlen( nuevo_tamanio_en_char );

        if (fcb_a_truncar_tamanio > nuevo_tamanio_archivo) {

            // CASO REDUCIR EL TAMANIO DEL ARCHIVO: TIENE QUE LIBERAR BLOQUES

            log_info(fs->logger, "Truncate resulta en REDUCIR. Tamanio actual es mayor al nuevo solicitado");
            log_info(fs->logger, "FCB ANTES: ");
            mostrar_info_fcb(fcb_a_truncar, fs->logger);
            mostrar_bloques_fcb(fcb_a_truncar->bloques, fs->logger);
            log_info(fs->logger, "Accedemos al bitmap y al archivo de bloques");

            int ultima_posicion_lista_bloques;
            uint32_t cant_bloques_a_liberar = (fcb_a_truncar_tamanio - nuevo_tamanio_archivo) / fs->block_size;

            for (uint32_t i = 0; i < cant_bloques_a_liberar; i++) {

                ultima_posicion_lista_bloques = list_size(fcb_a_truncar->bloques) - 1;
                uint32_t* bloque_a_liberar = (uint32_t*)list_get(fcb_a_truncar->bloques, ultima_posicion_lista_bloques);

                liberar_bloque(fs, bloque_a_liberar);
                list_remove(fcb_a_truncar->bloques, ultima_posicion_lista_bloques);

                log_info(fs->logger, "\e[1;92m---> Acceso Bloque - Archivo: <%s> - Bloque Archivo: <%d> - Bloque File System <%d>\e[0m", nombre_archivo_truncate, ultima_posicion_lista_bloques, (int)(*bloque_a_liberar));
            
            }            

            char* retorno_reducir = realloc(fcb_a_truncar->tamanio_archivo, (longitud_nueva + 1) * sizeof(char));      
            if (retorno_reducir == NULL) {
                log_error(fs->logger, "Error en el realloc. %s", strerror(errno));
            } else {
                fcb_a_truncar->tamanio_archivo = retorno_reducir;
                strcpy(fcb_a_truncar->tamanio_archivo, nuevo_tamanio_en_char );
            }

            config_set_value(fcb_a_truncar->fcb_config, "NOMBRE_ARCHIVO", fcb_a_truncar->nombre_archivo);
            config_set_value(fcb_a_truncar->fcb_config, "TAMANIO_ARCHIVO", fcb_a_truncar->tamanio_archivo);            
            config_set_value(fcb_a_truncar->fcb_config, "PUNTERO_DIRECTO", string_itoa(fcb_a_truncar->puntero_directo) );
            config_set_value(fcb_a_truncar->fcb_config, "PUNTERO_INDIRECTO", string_itoa(fcb_a_truncar->puntero_indirecto) );
            config_save(fcb_a_truncar->fcb_config);

        } else {

            // CASO AMPLIAR EL TAMANIO DEL ARCHIVO: TIENE QUE ASIGNAR BLOQUES

            log_info(fs->logger, "Truncate resulta en AMPLIAR. Tamanio actual es menor al nuevo solicitado");
            log_info(fs->logger, "FCB ANTES: ");
            mostrar_info_fcb(fcb_a_truncar, fs->logger);
            mostrar_bloques_fcb(fcb_a_truncar->bloques, fs->logger);
            log_info(fs->logger, "Accedemos al bitmap y al archivo de bloques");

            uint32_t bloque_libre;
            uint32_t cant_bloques_necesarios = (nuevo_tamanio_archivo - fcb_a_truncar_tamanio) / fs->block_size;

            if (list_size(fcb_a_truncar->bloques) == 0) {

                buscar_bloque_libre(fs, &bloque_libre);
                fcb_a_truncar->puntero_directo = bloque_libre;
                cant_bloques_necesarios--;

                log_info(fs->logger, "\e[1;92m---> Acceso Bloque - Archivo: <%s> - Bloque Archivo: <1> - Bloque File System <%d>\e[0m", nombre_archivo_truncate, bloque_libre);
            }

            for (uint32_t i = 0; i < cant_bloques_necesarios; i++) {
                
                buscar_bloque_libre(fs, &bloque_libre);
                if (fcb_a_truncar->puntero_indirecto == 0) {
                    fcb_a_truncar->puntero_indirecto = bloque_libre;
                }

                uint32_t* nuevo_bloque = malloc(sizeof(uint32_t)); // IMPORTANTE: PARA NO APUNTAR SIEMPRE AL MISMO PUNTERO
                *nuevo_bloque = bloque_libre; // IMPORTANTE: PARA NO APUNTAR SIEMPRE AL MISMO PUNTERO
                list_add(fcb_a_truncar->bloques, nuevo_bloque);

                log_info(fs->logger, "\e[1;92m---> Acceso Bloque - Archivo: <%s> - Bloque Archivo: <%d> - Bloque File System <%d>\e[0m", nombre_archivo_truncate, list_size(fcb_a_truncar->bloques), bloque_libre);
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
            config_set_value( fcb_a_truncar->fcb_config, "PUNTERO_DIRECTO", string_itoa( (int)fcb_a_truncar->puntero_directo )) ;
            config_set_value(fcb_a_truncar->fcb_config, "PUNTERO_INDIRECTO", string_itoa( (int)fcb_a_truncar->puntero_indirecto ));
            config_save(fcb_a_truncar->fcb_config);

        }

        log_info(fs->logger, "Salimos del bitmap y el archivo de bloques");
        log_info(fs->logger, "\e[1;92mTruncar Archivo: <%s> - Tamaño: <%d>\e[0m", nombre_archivo_truncate, nuevo_tamanio_archivo);
        log_info(fs->logger, "FCB DESPUES: ");
        mostrar_info_fcb(fcb_a_truncar, fs->logger);
        mostrar_bloques_fcb(fcb_a_truncar->bloques, fs->logger);
        log_info(fs->logger, "Archivo <%s> truncado correctamente", nombre_archivo_truncate);

        free(nuevo_tamanio_en_char);
        truncado_ok = 1;        
    }

    return truncado_ok;
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
