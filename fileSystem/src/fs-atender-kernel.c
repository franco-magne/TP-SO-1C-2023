#include "../include/fs-atender-kernel.h"

t_list* lista_fcbs;

/*------------------------------------------------------------------------- ATENDER KERNEL ----------------------------------------------------------------------------- */

void atender_kernel(t_filesystem* fs) {

    int operacion_OK = 0;
    lista_fcbs = list_create();
    
    
    
    crear_archivo("Testeando", fs);
    //abrir_archivo("RecuperatorioSO", fs);

    
    operacion_OK = truncar_archivo("Testeando", 256, fs);
    if (operacion_OK) {
        log_info(fs->logger, "Truncado ok");
    } else {
        log_error(fs->logger, "Algo feo paso");
    }
    
    

    while (fs->socket_kernel != -1) {
        
        log_info(fs->logger, "Esperando peticion de KERNEL...");
        uint8_t header = stream_recv_header(fs->socket_kernel); // RECIBO LA OPERACION QUE KERNEL QUIERA SOLICITAR        

        switch(header) {

            case HEADER_f_open:                
                                        
                char* nombre_archivo_open;
                t_buffer* buffer_nombre_archivo_open = buffer_create();                

                stream_recv_buffer(fs->socket_kernel, buffer_nombre_archivo_open); // RECIBO EL BUFFER NOMBRE DE ARCHIVO DE KERNEL
                nombre_archivo_open = buffer_unpack_string(buffer_nombre_archivo_open); // DESERIALIZO EL BUFFER MANDADO POR KERNEL

                log_info(fs->logger, "Recibo operacion F_OPEN <%s> de KERNEL", nombre_archivo_open);

                if (buscar_archivo(nombre_archivo_open)) {
                    // CASO: EL ARCHIVO YA EXISTE, TIENE SU FCB CREADO

                    log_info(fs->logger, "El FCB ya habia sido creado.");
                    log_info(fs->logger, "\e[1;92mAbrir archivo: <%s>\e[0m", nombre_archivo_open);
                    stream_send_empty_buffer(fs->socket_kernel, HANDSHAKE_ok_continue); // NOTIFICO A KERNEL QUE EL ARCHIVO EXISTE Y LO AGREGUE A SU TABLA GLOBAL

                } else {

                    operacion_OK = abrir_archivo(nombre_archivo_open, fs);
                    if (operacion_OK) {
                        // CASO: EL ARCHIVO NO EXISTE EN NUESTRO DIRECTORIO Y HAY QUE CREARLO PARA ABRIRLO

                        log_info(fs->logger, "\e[1;92mAbrir archivo: <%s>\e[0m", nombre_archivo_open);
                        stream_send_empty_buffer(fs->socket_kernel, HANDSHAKE_ok_continue); // NOTIFICO A KERNEL QUE EL ARCHIVO EXISTE Y LO AGREGUE A SU TABLA GLOBAL
                    } else {
                        // CASO: NO EXISTE ESE ARCHIVO. TIENE QUE SOLICITAR CREARLO PARA AGREGARLO A NUESTRO DIRECTORIO

                        stream_send_empty_buffer(fs->socket_kernel, HEADER_f_create);
                    }
                }

                free(nombre_archivo_open);
                buffer_destroy(buffer_nombre_archivo_open);

            break;

            case HEADER_f_create:

                char* nombre_archivo_create;
                t_buffer* buffer_nombre_archivo_create = buffer_create();

                stream_recv_buffer(fs->socket_kernel, buffer_nombre_archivo_create); // RECIBO EL BUFFER NOMBRE DE ARCHIVO DE KERNEL
                nombre_archivo_create = buffer_unpack_string(buffer_nombre_archivo_create); // DESERIALIZO EL BUFFER MANDADO POR KERNEL

                log_info(fs->logger, "Recibo operacion F_CREATE <%s> de KERNEL", nombre_archivo_create);

                operacion_OK = crear_archivo(nombre_archivo_create, fs);
                if (operacion_OK) {

                    log_info(fs->logger, "\e[1;92mCrear archivo: <%s>\e[0m", nombre_archivo_create);
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

                log_info(fs->logger, "Recibo operacion F_TRUNCATE <%s, %d> de KERNEL", nombre_archivo_truncate, tamanio_archivo_truncate);

                operacion_OK = truncar_archivo(nombre_archivo_truncate, tamanio_archivo_truncate, fs);
                if (operacion_OK) {

                    log_info(fs->logger, "\e[1;92mTruncar Archivo: <%s> - Tamaño: <%d>\e[0m", nombre_archivo_truncate, tamanio_archivo_truncate);
                    stream_send_empty_buffer(fs->socket_kernel, HANDSHAKE_ok_continue); // NOTIFICO A KERNEL QUE EL ARCHIVO SE TRUNCO

                } else {

                    log_info(fs->logger, "Error al truncar. Algo paso.");
                    stream_send_empty_buffer(fs->socket_kernel, HEADER_error); // FALLO EN EL TRUNCATE

                }  

                free(nombre_archivo_truncate);
                buffer_destroy(bufferTruncate);

            break;

            case HEADER_f_read:

                log_info(fs->logger, "Recibo operacion F_READ de KERNEL");



            break;

            case HEADER_f_write:

                log_info(fs->logger, "Recibo operacion F_WRITE de KERNEL");

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

/*------------------------------------------------------------------------- F_TRUNCATE ----------------------------------------------------------------------------- */

int truncar_archivo(char* nombre_archivo, uint32_t nuevo_tamanio_archivo, t_filesystem* fs) {

    int truncado_ok;
    int pos_archivo_a_truncar = -1;
    int size_lista_fcbs = list_size(lista_fcbs);
    
    for (int i = 0; i < size_lista_fcbs; i++) {

        t_fcb* fcb_aux = list_get(lista_fcbs, i);

        if (strcmp(fcb_aux->nombre_archivo, nombre_archivo) == 0) {
            pos_archivo_a_truncar = i;
        }
    }

    if (pos_archivo_a_truncar == -1) {
        truncado_ok = 0; // NO SE ENCONTRO UN ARCHIVO CON ESE NOMBRE
    } else {        

        t_fcb* fcb_a_truncar = list_get(lista_fcbs, pos_archivo_a_truncar);

        uint32_t fcb_a_truncar_tamanio = atoi(fcb_a_truncar->tamanio_archivo);
        //int cant_bloques_actuales = fcb_a_truncar_tamanio / fs->block_size;

        if (fcb_a_truncar_tamanio > nuevo_tamanio_archivo) {

            // CASO REDUCIR EL TAMANIO DEL ARCHIVO: TIENE QUE LIBERAR BLOQUES

            log_info(fs->logger, "Truncate resulta en REDUCIR. Tamanio actual es mayor al nuevo solicitado");
            log_info(fs->logger, "BEFORE: Tengo %d bloques y tamanio %d", list_size(fcb_a_truncar->bloques), fcb_a_truncar_tamanio);

            int ultima_posicion_lista_bloques;
            uint32_t cant_bloques_a_liberar = (fcb_a_truncar_tamanio - nuevo_tamanio_archivo) / fs->block_size;

            for (uint32_t i = 0; i < cant_bloques_a_liberar; i++) {
                ultima_posicion_lista_bloques = list_size(fcb_a_truncar->bloques) - 1;
                uint32_t* bloque_a_liberar = (uint32_t*)list_get(fcb_a_truncar->bloques, ultima_posicion_lista_bloques);

                liberar_bloque(fs, bloque_a_liberar);
                list_remove(fcb_a_truncar->bloques, ultima_posicion_lista_bloques);

                log_info(fs->logger, "\e[1;92mAcceso Bloque - Archivo: <%s> - Bloque Archivo: <%d> - Bloque File System <%d>\e[0m", nombre_archivo, ultima_posicion_lista_bloques, (int)(*bloque_a_liberar));
            }

            strcpy(fcb_a_truncar->tamanio_archivo, string_itoa(nuevo_tamanio_archivo));
            config_set_value(fcb_a_truncar->fcb_config, "TAMANIO_ARCHIVO", fcb_a_truncar->tamanio_archivo);

            config_save(fcb_a_truncar->fcb_config);
            log_info(fs->logger, "AFTER: Tengo %d bloques y tamanio %s", list_size(fcb_a_truncar->bloques), fcb_a_truncar->tamanio_archivo);

        } else {

            // CASO AMPLIAR EL TAMANIO DEL ARCHIVO: TIENE QUE ASIGNAR BLOQUES

            log_info(fs->logger, "Truncate resulta en AMPLIAR. Tamanio actual es menor al nuevo solicitado");
            log_info(fs->logger, "BEFORE: Tengo %d bloques y tamanio %d", list_size(fcb_a_truncar->bloques), fcb_a_truncar_tamanio);

            uint32_t bloque_filesystem;
            uint32_t cant_bloques_necesarios = (nuevo_tamanio_archivo - fcb_a_truncar_tamanio) / fs->block_size;

            if (list_size(fcb_a_truncar->bloques) == 0) {
                uint32_t puntero_directo_inicial = buscar_bloque_libre(fs, &bloque_filesystem);

                log_info(fs->logger, "Puntero directo es %d", puntero_directo_inicial );
                
                list_add(fcb_a_truncar->bloques, &puntero_directo_inicial);                
                fcb_a_truncar->puntero_directo = puntero_directo_inicial; // ------------------------------------------------------------------- VALGRIND MARCA ALGO EN ESTA LINEA                
                config_set_value( fcb_a_truncar->fcb_config, "PUNTERO_DIRECTO", string_itoa( fcb_a_truncar->puntero_directo )) ; // ------------------------------------------------------------------- VALGRIND MARCA ALGO EN ESTA LINEA
                cant_bloques_necesarios--;

                log_info(fs->logger, "\e[1;92mAcceso Bloque - Archivo: <%s> - Bloque Archivo: <1> - Bloque File System <%d>\e[0m", nombre_archivo, bloque_filesystem);
            }

            for (uint32_t i = 0; i < cant_bloques_necesarios; i++) {
                
                uint32_t bloque_nuevo = buscar_bloque_libre(fs, &bloque_filesystem);
                if (fcb_a_truncar->puntero_indirecto == 0) {
                    fcb_a_truncar->puntero_indirecto = bloque_nuevo;
                }
                list_add(fcb_a_truncar->bloques, &bloque_nuevo);

                log_info(fs->logger, "\e[1;92mAcceso Bloque - Archivo: <%s> - Bloque Archivo: <%d> - Bloque File System <%d>\e[0m", nombre_archivo, list_size(fcb_a_truncar->bloques), bloque_filesystem);
            }

            config_set_value(fcb_a_truncar->fcb_config, "PUNTERO_INDIRECTO", string_itoa( fcb_a_truncar->puntero_indirecto )); // ------------------------------------------------------------------- VALGRIND MARCA ALGO EN ESTA LINEA                

            size_t longitud_nueva = strlen( string_itoa(nuevo_tamanio_archivo) );
            void* retorno = realloc(fcb_a_truncar->tamanio_archivo, (longitud_nueva + 1) * sizeof(char));      
            if (retorno == NULL) {
                log_error(fs->logger, "Error en el realloc. %s", strerror(errno));
            }

            strcpy(fcb_a_truncar->tamanio_archivo, string_itoa(nuevo_tamanio_archivo) ); // ------------------------------------------------------------------- VALGRIND MARCA ALGO EN ESTA LINEA                
            config_set_value(fcb_a_truncar->fcb_config, "TAMANIO_ARCHIVO", fcb_a_truncar->tamanio_archivo); // ------------------------------------------------------------------- VALGRIND MARCA ALGO EN ESTA LINEA                

            config_save(fcb_a_truncar->fcb_config);
            log_info(fs->logger, "AFTER: Tengo %d bloques y tamanio %s", list_size(fcb_a_truncar->bloques), fcb_a_truncar->tamanio_archivo); // ------------------------------------------------------------------- VALGRIND MARCA ALGO EN ESTA LINEA                

        }

        truncado_ok = 1;
    }

    return truncado_ok;
}

/*------------------------------------------------------------------------- F_CREATE ----------------------------------------------------------------------------- */

int crear_archivo(char* nombre_archivo, t_filesystem* fs) {

    int resultado = 0;
    t_fcb* fcb_nuevo = crear_fcb_inexistente(nombre_archivo, fs);

    if (fcb_nuevo != NULL) {
        resultado = 1;
        list_add(lista_fcbs, fcb_nuevo);
    }     

    return resultado;
}

/*------------------------------------------------------------------------- F_OPEN ----------------------------------------------------------------------------- */

int abrir_archivo(char* nombre_archivo, t_filesystem* fs) {
    
    int resultado = 0;
    t_fcb* fcb_nuevo = crear_fcb(nombre_archivo, fs);

    if (fcb_nuevo != NULL) {
        resultado = 1;
        list_add(lista_fcbs, fcb_nuevo);

        //log_info(fs->logger, "Nombre: %s", fcb_nuevo->nombre_archivo);
        //log_info(fs->logger, "Tamanio: %s", fcb_nuevo->tamanio_archivo);
        //log_info(fs->logger, "Puntero directo: %d", fcb_nuevo->puntero_directo);
        //log_info(fs->logger, "Puntero inderecto: %d", fcb_nuevo->puntero_indirecto);

        //log_info(fs->logger, "Cantidad fcbs en lista: %d", list_size(lista_fcbs));
    }     

    return resultado;
}

int buscar_archivo(char* nombre_archivo) {

    int encontrado = 0;
    int size_lista_fcbs = list_size(lista_fcbs);
    
    for (int i = 0; i < size_lista_fcbs; i++) {

        t_fcb* fcb_aux;
        fcb_aux = list_get(lista_fcbs, i);

        if (strcmp(fcb_aux->nombre_archivo, nombre_archivo) == 0) {
            encontrado = 1;
        }
    }

    return encontrado;
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

    /*
    lista_bloques_actualizada = list_take_and_remove(fcb_a_truncar->bloques, cant_bloques_a_liberar); // QUITO LA CANT BLOQUES A LIBERAR DE LA DEL FCB Y LO GUARDO EN UNA NUEVA LISTA
    list_clean(fcb_a_truncar->bloques); // VACIO LA LISTA DE BLOQUES DEL FCB
    list_add_all(fcb_a_truncar->bloques, lista_bloques_actualizada); // COPIO LA NUEVA LISTA CON LOS BLOQUES LIBERADOS A LA PROPIA DEL FCB
    */
