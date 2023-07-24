#include "../include/fs-atender-kernel.h"

t_list *lista_fcbs;
int conexion_kernel = 1;
pthread_mutex_t mutexTest = PTHREAD_MUTEX_INITIALIZER;

/*------------------------------------------------------------------------- ATENDER KERNEL ----------------------------------------------------------------------------- */

int atender_kernel(t_filesystem *fs)
{

    conexion_kernel = 1;
    int operacion_OK = 0;    
    lista_fcbs = list_create();
    levantar_fcbs_del_directorio(fs, lista_fcbs);

    while (conexion_kernel)
    {
        
        log_info(fs->logger, CYAN BOLD "Esperando peticion de KERNEL...");
        uint8_t header = stream_recv_header(fs->socket_kernel); // RECIBO LA OPERACION QUE KERNEL QUIERA SOLICITAR
        pthread_mutex_lock(&mutexTest);

        switch (header)
        {

            case HEADER_f_open:

                char *nombre_archivo_open;
                t_buffer *bufferOpen = buffer_create();

                stream_recv_buffer(fs->socket_kernel, bufferOpen);      // RECIBO EL BUFFER NOMBRE DE ARCHIVO DE KERNEL
                nombre_archivo_open = buffer_unpack_string(bufferOpen); // DESERIALIZO EL BUFFER MANDADO POR KERNEL

                log_info(fs->logger, YELLOW BOLD "Recibo operacion F_OPEN <%s> de KERNEL", nombre_archivo_open);

                operacion_OK = abrir_archivo_fs(nombre_archivo_open, fs);
                if (operacion_OK)
                {
                    stream_send_empty_buffer(fs->socket_kernel, HANDSHAKE_ok_continue); // NOTIFICO A KERNEL QUE EL ARCHIVO EXISTE Y LO AGREGUE A SU TABLA GLOBAL
                }
                else
                {
                    log_info(fs->logger, "El archivo no existe. Solicite crearlo"); // NO EXISTE ESE ARCHIVO. TIENE QUE SOLICITAR CREARLO PARA AGREGARLO AL DIRECTORIO DE FCBs
                    stream_send_empty_buffer(fs->socket_kernel, HEADER_f_create);
                }

                free(nombre_archivo_open);
                buffer_destroy(bufferOpen);

            break;

            case HEADER_f_create:

                char *nombre_archivo_create;
                t_buffer *bufferCreate = buffer_create();

                stream_recv_buffer(fs->socket_kernel, bufferCreate);        // RECIBO EL BUFFER NOMBRE DE ARCHIVO DE KERNEL
                nombre_archivo_create = buffer_unpack_string(bufferCreate); // DESERIALIZO EL BUFFER MANDADO POR KERNEL

                log_info(fs->logger, YELLOW BOLD "Recibo operacion F_CREATE <%s> de KERNEL", nombre_archivo_create);

                operacion_OK = crear_archivo(nombre_archivo_create, fs);
                if (operacion_OK)
                {
                    stream_send_empty_buffer(fs->socket_kernel, HANDSHAKE_ok_continue);
                }

                free(nombre_archivo_create);
                buffer_destroy(bufferCreate);

            break;

            case HEADER_f_truncate:

                char *nombre_archivo_truncate;
                uint32_t tamanio_archivo_truncate;
                t_buffer *bufferTruncate = buffer_create();

                stream_recv_buffer(fs->socket_kernel, bufferTruncate);                                      // RECIBO EL BUFFER NOMBRE DE ARCHIVO DE KERNEL
                nombre_archivo_truncate = buffer_unpack_string(bufferTruncate);                             // DESERIALIZO EL BUFFER MANDADO POR KERNEL
                buffer_unpack(bufferTruncate, &tamanio_archivo_truncate, sizeof(tamanio_archivo_truncate)); // DESERIALIZO EL TAMANIO DE ARCHIVO

                log_info(fs->logger, YELLOW BOLD "Recibo operacion F_TRUNCATE <%s, %" PRIu32 "> de KERNEL", nombre_archivo_truncate, tamanio_archivo_truncate);

                operacion_OK = truncar_archivo(nombre_archivo_truncate, tamanio_archivo_truncate, fs);
                if (operacion_OK)
                {
                    stream_send_empty_buffer(fs->socket_kernel, HANDSHAKE_ok_continue); // NOTIFICO A KERNEL QUE EL ARCHIVO SE TRUNCO
                }
                else
                {
                    log_info(fs->logger, "Error al truncar");
                    stream_send_empty_buffer(fs->socket_kernel, HANDSHAKE_ok_continue); // FALLO EN EL TRUNCATE
                }

                free(nombre_archivo_truncate);
                buffer_destroy(bufferTruncate);

            break;

            case HEADER_f_read:

                char *nombre_archivo_read;
                uint32_t base_fisica_read;
                uint32_t desplazamiento_fisico_read;
                uint32_t cantidad_bytes_a_leer;
                uint32_t puntero_a_leer;
                
                t_buffer *bufferRead = buffer_create();

                stream_recv_buffer(fs->socket_kernel, bufferRead);
                nombre_archivo_read = buffer_unpack_string(bufferRead);
                buffer_unpack(bufferRead, &base_fisica_read, sizeof(base_fisica_read));
                buffer_unpack(bufferRead, &cantidad_bytes_a_leer, sizeof(cantidad_bytes_a_leer));
                buffer_unpack(bufferRead, &puntero_a_leer, sizeof(puntero_a_leer));
                buffer_unpack(bufferRead, &desplazamiento_fisico_read, sizeof(desplazamiento_fisico_read));

                log_info(fs->logger, YELLOW BOLD "Recibo operacion F_READ <%s, %" PRIu32 " puntero, [%" PRIu32 " | %" PRIu32 "], %" PRIu32 " bytes> de KERNEL", nombre_archivo_read, puntero_a_leer, base_fisica_read, desplazamiento_fisico_read, cantidad_bytes_a_leer);

                leer_archivo(nombre_archivo_read, base_fisica_read, desplazamiento_fisico_read, cantidad_bytes_a_leer, puntero_a_leer, fs);
                stream_send_empty_buffer(fs->socket_kernel, HANDSHAKE_ok_continue); // NOTIFICO A KERNEL QUE YA SE COMPLETO LA LECTURA

                free(nombre_archivo_read);
                buffer_destroy(bufferRead);

            break;

            case HEADER_f_write:

                char *nombre_archivo_write;
                uint32_t base_fisica_write;;
                uint32_t cantidad_bytes_a_escribir;
                uint32_t puntero_a_escribir;
                uint32_t desplazamiento_fisico_write;

                t_buffer *bufferWrite = buffer_create();

                stream_recv_buffer(fs->socket_kernel, bufferWrite);
                nombre_archivo_write = buffer_unpack_string(bufferWrite);
                buffer_unpack(bufferWrite, &base_fisica_write, sizeof(base_fisica_write));
                buffer_unpack(bufferWrite, &cantidad_bytes_a_escribir, sizeof(cantidad_bytes_a_escribir));
                buffer_unpack(bufferWrite, &puntero_a_escribir, sizeof(puntero_a_escribir));
                buffer_unpack(bufferWrite, &desplazamiento_fisico_write,sizeof(desplazamiento_fisico_write));

                log_info(fs->logger, YELLOW BOLD "Recibo operacion F_WRITE <%s, %" PRIu32 " puntero, [%" PRIu32 " | %" PRIu32 "], %" PRIu32 " bytes> de KERNEL", nombre_archivo_write, puntero_a_escribir, base_fisica_write, desplazamiento_fisico_write , cantidad_bytes_a_escribir);

                escribir_archivo(nombre_archivo_write, base_fisica_write, desplazamiento_fisico_write , cantidad_bytes_a_escribir, puntero_a_escribir, fs);
                stream_send_empty_buffer(fs->socket_kernel, HANDSHAKE_ok_continue); // NOTIFICO A KERNEL QUE YA SE COMPLETO LA ESCRITURA

                free(nombre_archivo_write);
                buffer_destroy(bufferWrite);

            break;

            case HEADER_Compactacion:

                log_info(fs->logger, YELLOW BOLD "Recibo operacion COMPACTACION");
                log_info(fs->logger, "Notifico a KERNEL que puede continuar");
                stream_recv_empty_buffer(fs->socket_kernel);
                stream_send_empty_buffer(fs->socket_kernel, HANDSHAKE_ok_continue);            

            break;

            default:

                log_info(fs->logger, "Hubo una desconexion");
                log_info(fs->logger, CYAN BOLD "Cliente KERNEL desconectado");
                conexion_kernel = 0;

            break;
        }

        pthread_mutex_unlock(&mutexTest);
        operacion_OK = 0;
    }    

    return 0;
}

/*------------------------------------------------------------------------- F_OPEN ----------------------------------------------------------------------------- */

int abrir_archivo_fs(char *nombre_archivo_open, t_filesystem *fs)
{

    int encontrado = 0;
    int size_lista_fcbs = list_size(lista_fcbs);

    for (int i = 0; i < size_lista_fcbs; i++)
    {

        t_fcb *fcb_aux = list_get(lista_fcbs, i);

        if (strcmp(fcb_aux->nombre_archivo, nombre_archivo_open) == 0)
        {

            log_info(fs->logger, GREEN BOLD "Abrir archivo: <%s>", nombre_archivo_open);
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

int crear_archivo(char *nombre_archivo_create, t_filesystem *fs)
{

    int resultado = 0;
    t_fcb *fcb_nuevo = crear_fcb_inexistente(nombre_archivo_create, fs);

    if (fcb_nuevo != NULL)
    {

        log_info(fs->logger, GREEN BOLD "Crear archivo: <%s>", nombre_archivo_create);
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

int truncar_archivo(char *nombre_archivo_truncate, uint32_t nuevo_tamanio_archivo, t_filesystem *fs)
{

    t_fcb *fcb_truncado;
    int truncado_ok = 0;
    int pos_archivo_a_truncar;
    int respuesta_afirmativa = 0;
    uint32_t fcb_a_truncar_tamanio_actual;
    int size_lista_fcbs = list_size(lista_fcbs);

    // BUSCO LA POSICION EN LA QUE SE ENCUENTRA EL FCB A TRUNCAR DENTRO DE LA LISTA DE FCBs
    for (int i = 0; i < size_lista_fcbs; i++)
    {

        t_fcb *fcb_aux = list_get(lista_fcbs, i);
        if (strcmp(fcb_aux->nombre_archivo, nombre_archivo_truncate) == 0)
        {
            fcb_a_truncar_tamanio_actual = atoi(fcb_aux->tamanio_archivo);

            if (nuevo_tamanio_archivo > fcb_a_truncar_tamanio_actual)
            {
                respuesta_afirmativa = puede_ampliar_tamanio(fcb_aux, fs->block_size, nuevo_tamanio_archivo, fcb_a_truncar_tamanio_actual);
            }

            pos_archivo_a_truncar = i;
        }
    }

    // PROCEDO A HACER EL TRUNCATE
    if (nuevo_tamanio_archivo < fcb_a_truncar_tamanio_actual)
    {

        fcb_truncado = reducir_tamanio_archivo(nombre_archivo_truncate, nuevo_tamanio_archivo, fs, pos_archivo_a_truncar);
        truncado_ok = 1;
    }
    else if (nuevo_tamanio_archivo > fcb_a_truncar_tamanio_actual && respuesta_afirmativa)
    {

        fcb_truncado = ampliar_tamanio_archivo(nombre_archivo_truncate, nuevo_tamanio_archivo, fs, pos_archivo_a_truncar);
        truncado_ok = 1;
    }

    if (truncado_ok)
    {

        log_info(fs->logger, "Salimos del bitmap y el archivo de bloques...");
        log_info(fs->logger, GREEN BOLD "Truncar Archivo: <%s> - Tamaño: <%" PRIu32 ">", nombre_archivo_truncate, nuevo_tamanio_archivo);
        log_info(fs->logger, "FCB DESPUES: ");
        mostrar_info_fcb(fcb_truncado, fs->logger);
        mostrar_bloques_fcb(fcb_truncado->bloques, fs->logger, fcb_truncado->puntero_directo);
        log_info(fs->logger, "Archivo <%s, %" PRIu32 "> truncado correctamente", nombre_archivo_truncate, nuevo_tamanio_archivo);
    }

    return truncado_ok;
}

t_fcb *ampliar_tamanio_archivo(char *nombre_archivo_truncate, uint32_t nuevo_tamanio_archivo, t_filesystem *fs, int pos_archivo_a_ampliar)
{

    t_fcb *fcb_a_truncar = list_get(lista_fcbs, pos_archivo_a_ampliar);
    uint32_t fcb_a_truncar_tamanio = atoi(fcb_a_truncar->tamanio_archivo);

    char *nuevo_tamanio_en_char = string_itoa(nuevo_tamanio_archivo);
    size_t longitud_nueva = strlen(nuevo_tamanio_en_char);

    log_info(fs->logger, "Truncate resulta en AMPLIAR");
    log_info(fs->logger, "FCB ANTES: ");
    mostrar_info_fcb(fcb_a_truncar, fs->logger);
    mostrar_bloques_fcb(fcb_a_truncar->bloques, fs->logger, fcb_a_truncar->puntero_directo);
    log_info(fs->logger, "Accedemos al bitmap y al archivo de bloques...");

    uint32_t bloque_libre;
    uint32_t cant_bloques_necesarios = (nuevo_tamanio_archivo - fcb_a_truncar_tamanio) / fs->block_size;

    // PUNTERO DIRECTO
    if (list_size(fcb_a_truncar->bloques) == 0)
    {

        buscar_bloque_libre(fs, &bloque_libre);
        fcb_a_truncar->puntero_directo = bloque_libre;
        cant_bloques_necesarios--;

        log_info(fs->logger, GREEN BOLD "Acceso Bloque - Archivo: <%s> - Bloque Archivo: <1> - Bloque File System <%" PRIu32 ">", nombre_archivo_truncate, bloque_libre);
    }

    if (nuevo_tamanio_archivo > fs->block_size)
    {

        // PUNTERO INDIRECTO Y BLOQUE DE PUNTEROS
        for (uint32_t i = 0; i <= cant_bloques_necesarios; i++)
        {

            buscar_bloque_libre(fs, &bloque_libre);
            if (fcb_a_truncar->puntero_indirecto == 0)
            {
                fcb_a_truncar->puntero_indirecto = bloque_libre;
            }

            uint32_t *nuevo_bloque = malloc(sizeof(uint32_t)); // IMPORTANTE: PARA NO APUNTAR SIEMPRE AL MISMO PUNTERO
            *nuevo_bloque = bloque_libre;                      // IMPORTANTE: PARA NO APUNTAR SIEMPRE AL MISMO PUNTERO
            list_add(fcb_a_truncar->bloques, nuevo_bloque);

            uint32_t puntero_numero_X = list_size(fcb_a_truncar->bloques) - 1; // RESTO UNO PORQUE NO TENGO QUE COPIAR EL PUNTERO INDIRECTO EN EL ARCHIVO DE BLOQUES
            escribir_bloque_de_punteros_en_puntero_indirecto(fcb_a_truncar->puntero_indirecto, puntero_numero_X, nuevo_bloque, fs->block_size);

            log_info(fs->logger, GREEN BOLD "Acceso Bloque - Archivo: <%s> - Bloque Archivo: <%d> - Bloque File System <%" PRIu32 ">", nombre_archivo_truncate, (list_size(fcb_a_truncar->bloques) + 1), bloque_libre);
        }
    }

    char *retorno_ampliar = realloc(fcb_a_truncar->tamanio_archivo, (longitud_nueva + 1) * sizeof(char));
    if (retorno_ampliar == NULL)
    {
        log_error(fs->logger, "Error en el realloc. %s", strerror(errno));
    }
    else
    {
        fcb_a_truncar->tamanio_archivo = retorno_ampliar;
        strcpy(fcb_a_truncar->tamanio_archivo, nuevo_tamanio_en_char);
    }

    persistir_fcb_config(fcb_a_truncar);
    free(nuevo_tamanio_en_char);

    return fcb_a_truncar;
}

int puede_ampliar_tamanio(t_fcb *fcb_a_ampliar, uint32_t block_size, uint32_t nuevo_tamanio, uint32_t fcb_tamanio_actual)
{

    int cantidad_bloques_fcb = list_size(fcb_a_ampliar->bloques) - 1;
    int cantidad_maxima_bloques = block_size / sizeof(uint32_t);
    int cantidad_bloques_necesarios = (nuevo_tamanio - fcb_tamanio_actual) / block_size;

    return (cantidad_bloques_fcb + cantidad_bloques_necesarios > cantidad_maxima_bloques) ? 0 : 1; // ? TRUE : FALSE
}

t_fcb *reducir_tamanio_archivo(char *nombre_archivo_truncate, uint32_t nuevo_tamanio_archivo, t_filesystem *fs, int pos_archivo_a_reducir)
{

    t_fcb *fcb_a_truncar = list_get(lista_fcbs, pos_archivo_a_reducir);
    uint32_t fcb_a_truncar_tamanio = atoi(fcb_a_truncar->tamanio_archivo);

    char *nuevo_tamanio_en_char = string_itoa(nuevo_tamanio_archivo);
    size_t longitud_nueva = strlen(nuevo_tamanio_en_char);

    log_info(fs->logger, "Truncate resulta en REDUCIR");
    log_info(fs->logger, "FCB ANTES: ");
    mostrar_info_fcb(fcb_a_truncar, fs->logger);
    mostrar_bloques_fcb(fcb_a_truncar->bloques, fs->logger, fcb_a_truncar->puntero_directo);
    log_info(fs->logger, "Accedemos al bitmap y al archivo de bloques...");

    int ultima_posicion_lista_bloques;
    uint32_t cant_bloques_a_liberar = (fcb_a_truncar_tamanio - nuevo_tamanio_archivo) / fs->block_size;

    if (nuevo_tamanio_archivo == 0)
    {
        cant_bloques_a_liberar -= 1;
    }

    for (uint32_t i = 0; i < cant_bloques_a_liberar; i++)
    {

        ultima_posicion_lista_bloques = list_size(fcb_a_truncar->bloques) - 1;
        uint32_t *bloque_a_liberar = (uint32_t *)list_get(fcb_a_truncar->bloques, ultima_posicion_lista_bloques);

        liberar_bloque(fs, bloque_a_liberar);
        list_remove(fcb_a_truncar->bloques, ultima_posicion_lista_bloques);
        liberar_puntero_del_bloque_de_punteros_en_puntero_indirecto(fcb_a_truncar->puntero_indirecto, ultima_posicion_lista_bloques, fs->block_size);

        log_info(fs->logger, GREEN BOLD "Acceso Bloque - Archivo: <%s> - Bloque Archivo: <%d> - Bloque File System <%d>", nombre_archivo_truncate, (ultima_posicion_lista_bloques + 1), (int)(*bloque_a_liberar));
    }

    char *retorno_reducir = realloc(fcb_a_truncar->tamanio_archivo, (longitud_nueva + 1) * sizeof(char));
    if (retorno_reducir == NULL)
    {
        log_error(fs->logger, "Error en el realloc. %s", strerror(errno));
    }
    else
    {
        fcb_a_truncar->tamanio_archivo = retorno_reducir;
        strcpy(fcb_a_truncar->tamanio_archivo, nuevo_tamanio_en_char);
    }

    if (nuevo_tamanio_archivo == fs->block_size)
    {
        fcb_a_truncar->puntero_indirecto = 0;
    }

    if (nuevo_tamanio_archivo == 0)
    {
        liberar_bloque(fs, &fcb_a_truncar->puntero_directo);
        fcb_a_truncar->puntero_directo = 0;
        fcb_a_truncar->puntero_indirecto = 0;
    }

    persistir_fcb_config(fcb_a_truncar);

    free(nuevo_tamanio_en_char);

    return fcb_a_truncar;
}

void persistir_fcb_config(t_fcb *fcb_truncado)
{
    char* puntero_directo_str = string_itoa(fcb_truncado->puntero_directo);
    char* puntero_indirecto_str = string_itoa(fcb_truncado->puntero_indirecto);

    config_set_value(fcb_truncado->fcb_config, "NOMBRE_ARCHIVO", fcb_truncado->nombre_archivo);
    config_set_value(fcb_truncado->fcb_config, "TAMANIO_ARCHIVO", fcb_truncado->tamanio_archivo);
    config_set_value(fcb_truncado->fcb_config, "PUNTERO_DIRECTO", puntero_directo_str); 
    config_set_value(fcb_truncado->fcb_config, "PUNTERO_INDIRECTO", puntero_indirecto_str);
    config_save(fcb_truncado->fcb_config);

    free(puntero_directo_str);
    free(puntero_indirecto_str);
}

/*------------------------------------------------------------------------- F_READ ----------------------------------------------------------------------------- */

void leer_archivo(char *nombre_archivo_read, uint32_t base_fisica, uint32_t desplazamiento_fisico, uint32_t cant_bytes_a_leer, uint32_t puntero_proceso, t_filesystem *fs)
{

    char *cadena_leida;
    int posicion_fcb_a_leer = devolver_posicion_fcb_en_la_lista(nombre_archivo_read);
    t_fcb *fcb_a_leer = list_get(lista_fcbs, posicion_fcb_a_leer);

    if (cant_bytes_a_leer <= fs->block_size)
    {

        cadena_leida = leer_archivo_bytes_menor_a_block_size(cant_bytes_a_leer, puntero_proceso, base_fisica, desplazamiento_fisico, fcb_a_leer, fs);
        enviar_informacion_a_memoria(base_fisica, desplazamiento_fisico, cadena_leida, cant_bytes_a_leer, fs);
    }
    else
    {

        cadena_leida = leer_archivo_bytes_mayor_a_block_size(cant_bytes_a_leer, puntero_proceso, base_fisica, desplazamiento_fisico, fcb_a_leer, fs);
        log_info(fs->logger, "Saliendo del bloque de punteros...");
        enviar_informacion_a_memoria(base_fisica, desplazamiento_fisico, cadena_leida, cant_bytes_a_leer, fs);
    }

    log_info(fs->logger, "<Archivo: %s, D.F.: %" PRIu32 ", %" PRIu32 " bytes> leido correctamente", nombre_archivo_read, base_fisica, cant_bytes_a_leer);

    free(cadena_leida);
}

char *leer_archivo_bytes_menor_a_block_size(uint32_t cant_bytes, uint32_t puntero, uint32_t base_fisica, uint32_t desplazamiento_fisico, t_fcb *fcb_a_leer, t_filesystem *fs)
{
    
    char *cadena_final;
    int posicion_puntero = puntero / fs->block_size;

    if (posicion_puntero == 0)
    {
        log_info(fs->logger, "Accediendo al puntero directo...");
        intervalo_de_pausa(fs->retardo_accesos);

        uint32_t bloque_lectura = fcb_a_leer->puntero_directo;        

        cadena_final = leer_puntero_del_archivo_de_bloques(bloque_lectura, cant_bytes, fs);        
        log_info(fs->logger, GREEN BOLD "Acceso Bloque - Archivo: <%s> - Bloque Archivo: <%d> - Bloque File System <%" PRIu32 ">", fcb_a_leer->nombre_archivo, 1, bloque_lectura);
        
        log_info(fs->logger, GREEN BOLD "Leer Archivo: <%s> - Puntero: <%" PRIu32 "> - Memoria: <%" PRIu32 "> - Tamaño: <%" PRIu32 ">", fcb_a_leer->nombre_archivo, puntero, desplazamiento_fisico, cant_bytes);
        log_info(fs->logger, "Saliendo del puntero directo...");
    }
    else
    {

        log_info(fs->logger, "Accediendo al bloque de punteros...");
        intervalo_de_pausa(fs->retardo_accesos);

        uint32_t *bloque_lectura = (uint32_t *)list_get(fcb_a_leer->bloques, posicion_puntero);        
        
        cadena_final = leer_puntero_del_archivo_de_bloques((*bloque_lectura), cant_bytes, fs);
        log_info(fs->logger, GREEN BOLD "Acceso Bloque - Archivo: <%s> - Bloque Archivo: <%d> - Bloque File System <%" PRIu32 ">", fcb_a_leer->nombre_archivo, posicion_puntero, (*bloque_lectura));
        
        log_info(fs->logger, GREEN BOLD "Leer Archivo: <%s> - Puntero: <%" PRIu32 "> - Memoria: <%" PRIu32 "> - Tamaño: <%" PRIu32 ">", fcb_a_leer->nombre_archivo, puntero, desplazamiento_fisico, cant_bytes);
        log_info(fs->logger, "Saliendo del bloque de punteros...");
    }

    return cadena_final;
}

char *leer_archivo_bytes_mayor_a_block_size(uint32_t cant_bytes, uint32_t puntero, uint32_t base_fisica, uint32_t desplazamiento_fisico, t_fcb *fcb_a_leer, t_filesystem *fs)
{

    int first_time = 1;
    char *cadena_final = malloc(cant_bytes); // malloc(cant_bytes + 1);
    int posicion_puntero = puntero / fs->block_size;
    int cant_bloques_a_leer = (int)ceil(cant_bytes / fs->block_size);

    uint32_t bytes_en_array[cant_bytes];
    convertir_cantidad_bytes_en_array(cant_bytes, bytes_en_array, fs->block_size);

    for (int i = 0; i < cant_bloques_a_leer; i++)
    {

        char *cadena_aux = malloc(bytes_en_array[i]);

        if (posicion_puntero == 0)
        {
            log_info(fs->logger, "Accediendo al puntero directo...");
            intervalo_de_pausa(fs->retardo_accesos);

            uint32_t bloque_lectura = fcb_a_leer->puntero_directo;
            log_info(fs->logger, GREEN BOLD "Acceso Bloque - Archivo: <%s> - Bloque Archivo: <%d> - Bloque File System <%" PRIu32 ">", fcb_a_leer->nombre_archivo, posicion_puntero, bloque_lectura);
            cadena_aux = leer_puntero_del_archivo_de_bloques(bloque_lectura, bytes_en_array[i], fs);
            string_append(&cadena_final, cadena_aux);

            log_info(fs->logger, "Saliendo del puntero directo...");
            log_info(fs->logger, GREEN BOLD "Leer Archivo: <%s> - Puntero: <%d> - Memoria: <%" PRIu32 "> - Tamaño: <%" PRIu32 ">", fcb_a_leer->nombre_archivo, posicion_puntero, base_fisica, bytes_en_array[i]);
        }
        else
        {

            if (first_time)
            {
                log_info(fs->logger, "Accediendo al bloque de punteros...");
                intervalo_de_pausa(fs->retardo_accesos);
                first_time = 0;
            }

            uint32_t *bloque_lectura = (uint32_t *)list_get(fcb_a_leer->bloques, puntero);
            log_info(fs->logger, GREEN BOLD "Acceso Bloque - Archivo: <%s> - Bloque Archivo: <%d> - Bloque File System <%" PRIu32 ">", fcb_a_leer->nombre_archivo, posicion_puntero, (*bloque_lectura));
            cadena_aux = leer_puntero_del_archivo_de_bloques((*bloque_lectura), bytes_en_array[i], fs);
            string_append(&cadena_final, cadena_aux);

            intervalo_de_pausa(fs->retardo_accesos);
            log_info(fs->logger, GREEN BOLD "Leer Archivo: <%s> - Puntero: <%d> - Memoria: <%" PRIu32 "> - Tamaño: <%" PRIu32 ">", fcb_a_leer->nombre_archivo, posicion_puntero, base_fisica, bytes_en_array[i]);
        }

        posicion_puntero++;
        free(cadena_aux);
    }

    return cadena_final;
}

void enviar_informacion_a_memoria(uint32_t base_fisica, uint32_t desplazamiento_fisico, char *cadena_leida, uint32_t cant_bytes_a_leer, t_filesystem *fs)
{

    t_buffer *bufferMemoria = buffer_create();

    log_info(fs->logger, MAGENTA BOLD "Enviando la cadena <%s> a MEMORIA", cadena_leida);

    buffer_pack(bufferMemoria, &base_fisica, sizeof(base_fisica));
    buffer_pack(bufferMemoria, &desplazamiento_fisico, sizeof(desplazamiento_fisico));
    buffer_pack(bufferMemoria, &cant_bytes_a_leer, sizeof(cant_bytes_a_leer));
    buffer_pack_string(bufferMemoria, cadena_leida);
    stream_send_buffer(fs->socket_memoria, HEADER_f_read, bufferMemoria);
    buffer_destroy(bufferMemoria);

    log_info(fs->logger, MAGENTA BOLD "Esperando finalizacion de MEMORIA...");

    uint8_t header_respuesta_memoria = stream_recv_header(fs->socket_memoria);
    stream_recv_empty_buffer(fs->socket_memoria);

    if (header_respuesta_memoria == HANDSHAKE_ok_continue)
    {
        log_info(fs->logger, MAGENTA BOLD "MEMORIA termina la escritura en la direccion fisica <%" PRIu32 ">", desplazamiento_fisico);
    }
}

// xxd -b -l 10 bitmap.dat

/*------------------------------------------------------------------------- F_WRITE ----------------------------------------------------------------------------- */

void escribir_archivo(char *nombre_archivo_write, uint32_t base_fisica, uint32_t desplazamiento_segmento, uint32_t cant_bytes_a_escribir, uint32_t puntero_proceso, t_filesystem *fs)
{

    char *respuesta_memoria; // malloc(cant_bytes_a_escribir);
    respuesta_memoria = pedir_informacion_a_memoria(base_fisica, desplazamiento_segmento,cant_bytes_a_escribir, fs);

    int posicion_fcb_a_escribir = devolver_posicion_fcb_en_la_lista(nombre_archivo_write);
    t_fcb *fcb_a_escribir = list_get(lista_fcbs, posicion_fcb_a_escribir);

    if (cant_bytes_a_escribir <= fs->block_size)
    {

        escribir_archivo_bytes_menor_a_block_size(cant_bytes_a_escribir, puntero_proceso, desplazamiento_segmento, fcb_a_escribir, fs, respuesta_memoria);
    }
    else
    {

        escribir_archivo_bytes_mayor_a_block_size(cant_bytes_a_escribir, puntero_proceso, desplazamiento_segmento, fcb_a_escribir, fs, respuesta_memoria);
    }

    log_info(fs->logger, "<Archivo: %s, D.F.: %" PRIu32 ", %" PRIu32 " bytes> escrito correctamente", nombre_archivo_write, desplazamiento_segmento, cant_bytes_a_escribir);

    free(respuesta_memoria);
}

void escribir_archivo_bytes_menor_a_block_size(uint32_t cant_bytes, uint32_t puntero, uint32_t direccion_fisica, t_fcb *fcb_a_escribir, t_filesystem *fs, char *respuesta_memoria)
{

    int posicion_puntero = puntero / fs->block_size;

    if (posicion_puntero == 0)
    { // ESCRIBO EN EL PUNTERO DIRECTO

        log_info(fs->logger, "Accediendo al puntero directo...");
        intervalo_de_pausa(fs->retardo_accesos);

        uint32_t bloque_escritura = fcb_a_escribir->puntero_directo;        
        escribir_en_puntero_del_archivo_de_bloques(bloque_escritura, cant_bytes, respuesta_memoria, fs);
        log_info(fs->logger, GREEN BOLD "Acceso Bloque - Archivo: <%s> - Bloque Archivo: <%d> - Bloque File System <%" PRIu32 ">", fcb_a_escribir->nombre_archivo, posicion_puntero, bloque_escritura);

        log_info(fs->logger, GREEN BOLD "Escribir Archivo: <%s> - Puntero: <%d> - Memoria: <%" PRIu32 "> - Tamaño: <%" PRIu32 ">", fcb_a_escribir->nombre_archivo, posicion_puntero, direccion_fisica, cant_bytes);
        log_info(fs->logger, "Saliendo del puntero directo...");
    }
    else
    { // ESCRIBO EN EL PUNTERO PASADO CONTENIDO EN EL PUNTERO INDIRECTO

        log_info(fs->logger, "Accediendo al bloque de punteros...");
        intervalo_de_pausa(fs->retardo_accesos);

        uint32_t *bloque_escritura = (uint32_t *)list_get(fcb_a_escribir->bloques, posicion_puntero);        
        escribir_en_puntero_del_archivo_de_bloques((*bloque_escritura), cant_bytes, respuesta_memoria, fs);
        log_info(fs->logger, GREEN BOLD "Acceso Bloque - Archivo: <%s> - Bloque Archivo: <%d> - Bloque File System <%" PRIu32 ">", fcb_a_escribir->nombre_archivo, posicion_puntero, (*bloque_escritura));

        log_info(fs->logger, GREEN BOLD "Escribir Archivo: <%s> - Puntero: <%d> - Memoria: <%" PRIu32 "> - Tamaño: <%" PRIu32 ">", fcb_a_escribir->nombre_archivo, posicion_puntero, direccion_fisica, cant_bytes);
        log_info(fs->logger, "Saliendo del bloque de punteros...");
    }
    
}

void escribir_archivo_bytes_mayor_a_block_size(uint32_t cant_bytes, uint32_t puntero, uint32_t direccion_fisica, t_fcb *fcb_a_escribir, t_filesystem *fs, char *respuesta_memoria)
{

    int first_time = 1;
    int posicion_puntero = puntero / fs->block_size;
    int cant_bloques_a_escribir = cant_bytes / fs->block_size;

    uint32_t bytes_en_array[cant_bloques_a_escribir];
    convertir_cantidad_bytes_en_array(cant_bytes, bytes_en_array, fs->block_size);

    for (int i = 0; i <= cant_bloques_a_escribir; i++)
    {

        if (posicion_puntero == 0)
        {
            char* cadena_temporal =  malloc(bytes_en_array[i]);

            log_info(fs->logger, "Accediendo al puntero directo...");
            intervalo_de_pausa(fs->retardo_accesos);

            uint32_t bloque_escritura = fcb_a_escribir->puntero_directo;            

            memcpy(cadena_temporal, respuesta_memoria, bytes_en_array[i]);
            escribir_en_puntero_del_archivo_de_bloques(bloque_escritura, bytes_en_array[i], cadena_temporal, fs);
            log_info(fs->logger, GREEN BOLD "Acceso Bloque - Archivo: <%s> - Bloque Archivo: <%d> - Bloque File System <%" PRIu32 ">", fcb_a_escribir->nombre_archivo, posicion_puntero, bloque_escritura);

            log_info(fs->logger, GREEN BOLD "Escribir Archivo: <%s> - Puntero: <%d> - Memoria: <%" PRIu32 "> - Tamaño: <%" PRIu32 ">", fcb_a_escribir->nombre_archivo, posicion_puntero, direccion_fisica, bytes_en_array[i]);
            log_info(fs->logger, "Saliendo del puntero directo...");

            free(cadena_temporal);
        }
        else
        {

            if (first_time)
            {
                log_info(fs->logger, "Accediendo al bloque de punteros...");
                intervalo_de_pausa(fs->retardo_accesos);
                first_time = 0;
            }

            char* otra_cadena = malloc(bytes_en_array[i]);
            uint32_t *bloque_escritura = (uint32_t *)list_get(fcb_a_escribir->bloques, (posicion_puntero -1));                        

            memcpy(otra_cadena, respuesta_memoria + (i * fs->block_size) , bytes_en_array[i]);
            escribir_en_puntero_del_archivo_de_bloques((*bloque_escritura), bytes_en_array[i], otra_cadena, fs);
            log_info(fs->logger, GREEN BOLD "Acceso Bloque - Archivo: <%s> - Bloque Archivo: <%d> - Bloque File System <%" PRIu32 ">", fcb_a_escribir->nombre_archivo, posicion_puntero, (*bloque_escritura));

            free(otra_cadena);

            intervalo_de_pausa(fs->retardo_accesos);
            log_info(fs->logger, GREEN BOLD "Escribir Archivo: <%s> - Puntero: <%d> - Memoria: <%" PRIu32 "> - Tamaño: <%" PRIu32 ">", fcb_a_escribir->nombre_archivo, posicion_puntero, direccion_fisica, bytes_en_array[i]);
        }

        posicion_puntero++;
    }

    log_info(fs->logger, "Saliendo del bloque de punteros...");
}

char *pedir_informacion_a_memoria(uint32_t base_segmento, uint32_t desplazamiento_segmento, uint32_t cant_bytes_necesarios, t_filesystem *fs)
{

    // ENVIO A MEMORIA LA DIRECCION FISICA Y CANTIDAD DE BYTES

    t_buffer *bufferMemoria = buffer_create();

    log_info(fs->logger, MAGENTA BOLD "Solicito a MEMORIA la informacion de la direccion fisica %" PRIu32 "", desplazamiento_segmento);
    buffer_pack(bufferMemoria, &base_segmento, sizeof(base_segmento));
    buffer_pack(bufferMemoria, &desplazamiento_segmento, sizeof(desplazamiento_segmento) );
    buffer_pack(bufferMemoria, &cant_bytes_necesarios, sizeof(cant_bytes_necesarios) );
    stream_send_buffer(fs->socket_memoria, HEADER_f_write, bufferMemoria);    

    buffer_destroy(bufferMemoria);

    // ESPERO LA RESPUESTA DE MEMORIA CON LO LEIDO

    t_buffer *buffer_respuesta_memoria = buffer_create();

    log_info(fs->logger, MAGENTA BOLD "Esperando respuesta de MEMORIA...");
    stream_recv_header(fs->socket_memoria);
    stream_recv_buffer(fs->socket_memoria, buffer_respuesta_memoria);
    char *respuesta_memoria = buffer_unpack_string(buffer_respuesta_memoria);
    log_info(fs->logger, MAGENTA BOLD "Recibo de MEMORIA: %s", respuesta_memoria);

    buffer_destroy(buffer_respuesta_memoria);

    return respuesta_memoria;
}

/*------------------------------------------------------------------------- ESPERAR KERNEL ----------------------------------------------------------------------------- */

int fs_escuchando_en(int server_fs, t_filesystem *fs) {

    pthread_t hilo;
    int socket_kernel = esperar_cliente(server_fs);

    fs->socket_kernel = socket_kernel;
    log_info(fs->logger, CYAN BOLD "Cliente KERNEL conectado");

    if (socket_kernel != -1) {

        pthread_create(&hilo, NULL, (void *)atender_kernel, (void *)fs);
        pthread_join(hilo, NULL);
    }

    return (!conexion_kernel) ? 0 : 1;
}

/*------------------------------------------------------------------------- OTRAS ----------------------------------------------------------------------------- */

int devolver_posicion_fcb_en_la_lista(char *nombre_archivo)
{

    int posicion_fcb;
    int size_lista_fcbs = list_size(lista_fcbs);

    for (int i = 0; i < size_lista_fcbs; i++)
    {

        t_fcb *fcb_aux = list_get(lista_fcbs, i);
        if (strcmp(fcb_aux->nombre_archivo, nombre_archivo) == 0)
        {
            posicion_fcb = i;

            break;
        }
    }

    return posicion_fcb;
}

void convertir_cantidad_bytes_en_array(uint32_t cantidad_bytes, uint32_t *array_bytes, uint32_t block_size)
{

    uint32_t bytes_restantes = cantidad_bytes % block_size;
    int cantidad_block_size_repetido = cantidad_bytes / block_size;

    for (int i = 0; i < cantidad_block_size_repetido; i++)
    {
        array_bytes[i] = block_size;
    }

    array_bytes[cantidad_block_size_repetido] = bytes_restantes; // LA POSICION FINAL GUARDO EL RESTO DE BYTES QUE QUEDAN
}
