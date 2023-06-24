#include "../include/fs-atender-kernel.h"

t_list* lista_fcbs;

void atender_kernel(t_filesystem* fs) {

    int operacion_OK = 0;
    lista_fcbs = list_create();
    
    // PARA HACER PRUEBAS RAPIDAS
    /*
    abrir_archivo("Notas1erParcialK9999", fs);    
    operacion_OK = truncar_archivo("Notas1erParcialK9999", 256, fs);
    if (operacion_OK) {
        log_info(fs->logger, "Truncado ok");
    } else {
        log_error(fs->logger, "Algo feo paso");
    }
    //operacion_OK = abrir_archivo("RecuperatorioSO", fs);
    */
    while (fs->socket_kernel != -1) {
        
        log_info(fs->logger, "Esperando peticion de KERNEL...");
        uint8_t header = stream_recv_header(fs->socket_kernel); // RECIBO LA OPERACION QUE KERNEL QUIERA SOLICITAR        

        switch(header) {

            case HEADER_f_open:

                log_info(fs->logger, "Recibo operacion F_OPEN de KERNEL.");
                                        
                char* nombre_archivo_open;
                t_buffer* buffer_nombre_archivo_open = buffer_create();

                stream_recv_buffer(fs->socket_kernel, buffer_nombre_archivo_open); // RECIBO EL BUFFER NOMBRE DE ARCHIVO DE KERNEL
                nombre_archivo_open = buffer_unpack_string(buffer_nombre_archivo_open); // DESERIALIZO EL BUFFER MANDADO POR KERNEL

                if (buscar_archivo(nombre_archivo_open)) {
                    // CASO: EL ARCHIVO YA EXISTE, TIENE SU FCB CREADO
                    log_info(fs->logger, "El FCB ya habia sido creado.");
                    log_info(fs->logger, "Abrir archivo: <%s>", nombre_archivo_open);
                    stream_send_empty_buffer(fs->socket_kernel, HANDSHAKE_ok_continue); // NOTIFICO A KERNEL QUE EL ARCHIVO EXISTE Y LO AGREGUE A SU TABLA GLOBAL
                } else {
                    operacion_OK = abrir_archivo(nombre_archivo_open, fs);
                    if (operacion_OK) {
                        // CASO: EL ARCHIVO NO EXISTE Y TENGO QUE CREAR EL FCB
                        log_info(fs->logger, "Se creo el FCB.");
                        log_info(fs->logger, "Abrir archivo: <%s>", nombre_archivo_open);
                        stream_send_empty_buffer(fs->socket_kernel, HANDSHAKE_ok_continue); // NOTIFICO A KERNEL QUE EL ARCHIVO EXISTE Y LO AGREGUE A SU TABLA GLOBAL
                    } else {
                        // CASO: NO EXISTE EL FCB DE ESE ARCHIVO. TIENE QUE SOLICITAR CREARLO
                        log_info(fs->logger, "Entro a solicitar una creacion");
                        stream_send_empty_buffer(fs->socket_kernel, HEADER_f_create);
                    }                                                                            
                }

                free(nombre_archivo_open);
                buffer_destroy(buffer_nombre_archivo_open);

            break;

            case HEADER_f_create:

                log_info(fs->logger, "Recibo operacion F_CREATE de KERNEL.");

                char* nombre_archivo_create;
                t_buffer* buffer_nombre_archivo_create = buffer_create();

                stream_recv_buffer(fs->socket_kernel, buffer_nombre_archivo_create); // RECIBO EL BUFFER NOMBRE DE ARCHIVO DE KERNEL
                nombre_archivo_create = buffer_unpack_string(buffer_nombre_archivo_create); // DESERIALIZO EL BUFFER MANDADO POR KERNEL

                operacion_OK = crear_archivo(nombre_archivo_create, fs);
                if (operacion_OK) {
                    log_info(fs->logger, "Crear archivo: <%s>", nombre_archivo_create);
                    stream_send_empty_buffer(fs->socket_kernel, HANDSHAKE_ok_continue);
                } else {
                    log_info(fs->logger, "Error al crear el nuevo archivo.");
                    stream_send_empty_buffer(fs->socket_kernel, HEADER_error);
                }

                free(nombre_archivo_create);
                buffer_destroy(buffer_nombre_archivo_create);

            break;

            case HEADER_f_truncate:

                log_info(fs->logger, "Recibo operacion F_TRUNCATE de KERNEL.");

                char* nombre_archivo_truncate;
                uint32_t tamanio_archivo_truncate;
                t_buffer* bufferTruncate = buffer_create();                

                stream_recv_buffer(fs->socket_kernel, bufferTruncate); // RECIBO EL BUFFER NOMBRE DE ARCHIVO DE KERNEL
                nombre_archivo_truncate = buffer_unpack_string(bufferTruncate); // DESERIALIZO EL BUFFER MANDADO POR KERNEL
                buffer_unpack(bufferTruncate, &tamanio_archivo_truncate, sizeof(tamanio_archivo_truncate)); // DESERIALIZO EL TAMANIO DE ARCHIVO

                operacion_OK = truncar_archivo(nombre_archivo_truncate, tamanio_archivo_truncate, fs);

                if (operacion_OK) {
                    log_info(fs->logger, "Truncar Archivo: <%s> - Tamaño: <%d>", nombre_archivo_truncate, tamanio_archivo_truncate);
                    stream_send_empty_buffer(fs->socket_kernel, HANDSHAKE_ok_continue); // NOTIFICO A KERNEL QUE EL ARCHIVO SE TRUNCO
                } else {
                    stream_send_empty_buffer(fs->socket_kernel, HEADER_error); // FALLO EN EL TRUNCATE
                }  

                free(nombre_archivo_truncate);
                buffer_destroy(bufferTruncate);

            break;

            case HEADER_f_read:

                log_info(fs->logger, "Recibo operacion F_READ de KERNEL.");



            break;

            case HEADER_f_write:

                log_info(fs->logger, "Recibo operacion F_WRITE de KERNEL.");

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

            int ultima_posicion_lista_bloques;
            uint32_t cant_bloques_a_liberar = (fcb_a_truncar_tamanio - nuevo_tamanio_archivo) / fs->block_size;

            for (uint32_t i = 0; i < cant_bloques_a_liberar; i++) {
                ultima_posicion_lista_bloques = list_size(fcb_a_truncar->bloques) - 1;
                uint32_t* bloque_a_liberar = (uint32_t*)list_get(fcb_a_truncar->bloques, ultima_posicion_lista_bloques);

                liberar_bloque(fs, bloque_a_liberar);
                list_remove(fcb_a_truncar->bloques, ultima_posicion_lista_bloques);

                log_info(fs->logger, "Acceso Bloque - Archivo: <%s> - Bloque Archivo: <%d> - Bloque File System <%d>", nombre_archivo, ultima_posicion_lista_bloques, (int)(*bloque_a_liberar));
            }

            fcb_a_truncar->tamanio_archivo = string_itoa(nuevo_tamanio_archivo);
            config_set_value(fcb_a_truncar->fcb_config, "TAMANIO_ARCHIVO", string_itoa(nuevo_tamanio_archivo));

            config_save(fcb_a_truncar->fcb_config);

        } else {

            // CASO AMPLIAR EL TAMANIO DEL ARCHIVO: TIENE QUE ASIGNAR BLOQUES

            uint32_t bloque_filesystem;
            uint32_t cant_bloques_necesarios = (fcb_a_truncar_tamanio - nuevo_tamanio_archivo) / fs->block_size;

            if (list_size(fcb_a_truncar->bloques) == 0) {
                uint32_t* puntero_directo_inicial = buscar_bloque_libre(fs, &bloque_filesystem);
                list_add(fcb_a_truncar->bloques, puntero_directo_inicial);
                fcb_a_truncar->puntero_directo = (*puntero_directo_inicial);
                config_set_value(fcb_a_truncar->fcb_config, "PUNTERO_DIRECTO", string_itoa((int)(*puntero_directo_inicial)));
                cant_bloques_necesarios--;

                log_info(fs->logger, "Acceso Bloque - Archivo: <%s> - Bloque Archivo: <1> - Bloque File System <%d>", nombre_archivo, bloque_filesystem);
            }

            for (uint32_t i = 0; i < cant_bloques_necesarios; i++) {
                uint32_t* bloque_nuevo = buscar_bloque_libre(fs, &bloque_filesystem);
                list_add(fcb_a_truncar->bloques, bloque_nuevo);

                log_info(fs->logger, "Acceso Bloque - Archivo: <%s> - Bloque Archivo: <%d> - Bloque File System <%d>", nombre_archivo, list_size(fcb_a_truncar->bloques), bloque_filesystem);
            }

            uint32_t* primer_puntero_indirecto = list_get(fcb_a_truncar->bloques, 1); // EL PUNTERO INDIRECTO ES PRIMER PUNTERO EN LA POSICION 2
            fcb_a_truncar->puntero_indirecto = (*primer_puntero_indirecto);
            config_set_value(fcb_a_truncar->fcb_config, "PUNTERO_INDIRECTO", string_itoa((int)(*primer_puntero_indirecto)));

            fcb_a_truncar->tamanio_archivo = string_itoa(nuevo_tamanio_archivo);
            config_set_value(fcb_a_truncar->fcb_config, "TAMANIO_ARCHIVO", string_itoa(nuevo_tamanio_archivo));

            config_save(fcb_a_truncar->fcb_config);
        }

        truncado_ok = 1;
    }

    return truncado_ok;
}

/*------------------------------------------------------------------------- F_CREATE ----------------------------------------------------------------------------- */

int crear_archivo(char* nombre_archivo, t_filesystem* fs) {

    int resultado = 0;
    t_fcb* fcb_nuevo = malloc(sizeof(t_fcb));

    fcb_nuevo = crear_fcb_inexistente(nombre_archivo, fs);
    if (fcb_nuevo != NULL) {
        resultado = 1;
        list_add(lista_fcbs, fcb_nuevo);
    }     

    return resultado;
}

/*------------------------------------------------------------------------- F_OPEN ----------------------------------------------------------------------------- */

int abrir_archivo(char* nombre_archivo, t_filesystem* fs) {
    
    int resultado = 0;
    t_fcb* fcb_nuevo = malloc(sizeof(t_fcb));

    fcb_nuevo = crear_fcb(nombre_archivo, fs);
    if (fcb_nuevo != NULL) {
        resultado = 1;
        list_add(lista_fcbs, fcb_nuevo);
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
    
        pthread_create(&hilo, NULL, (void*) atender_kernel, fs);
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
