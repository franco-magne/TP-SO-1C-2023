#include "../include/fs-atender-kernel.h"

t_list* lista_fcbs;

void atender_kernel(t_filesystem* fs) {

    int operacion_OK = 0;
    lista_fcbs = list_create();
    /* CON ESTO TESTEABA
    operacion_OK = abrir_archivo("Notas1erParcialK9999", fs);
    if (operacion_OK) {
        log_info(fs->logger, "Apertura ok");
    } else {
        log_error(fs->logger, "error al abrir");
    }*/
    
    while (fs->socket_kernel != -1) {

        log_info(fs->logger, "Esperando peticion de KERNEL...");
        uint8_t header = stream_recv_header(fs->socket_kernel); // RECIBO LA OPERACION QUE KERNEL QUIERA SOLICITAR

        switch(header) {
            case HEADER_f_open:
                                        
                char* nombre_archivo_open;
                t_buffer* buffer_nombre_archivo_open = buffer_create();

                stream_recv_buffer(fs->socket_kernel, buffer_nombre_archivo_open); // RECIBO EL BUFFER NOMBRE DE ARCHIVO DE KERNEL
                nombre_archivo_open = buffer_unpack_string(buffer_nombre_archivo_open); // DESERIALIZO EL BUFFER MANDADO POR KERNEL

                if (buscar_archivo(nombre_archivo_open)) {
                    // CASO: EL ARCHIVO YA EXISTE, TIENE SU FCB CREADO
                    log_info(fs->logger, "Abrir archivo: <%s>", nombre_archivo_open);
                    stream_send_empty_buffer(fs->socket_kernel, HANDSHAKE_ok_continue); // NOTIFICO A KERNEL QUE EL ARCHIVO EXISTE Y LO AGREGUE A SU TABLA GLOBAL
                } else {
                    operacion_OK = abrir_archivo(nombre_archivo_open, fs);
                    if (operacion_OK) {
                        // CASO: EL ARCHIVO NO EXISTE Y TENGO QUE CREAR EL FCB
                        log_info(fs->logger, "Abrir archivo: <%s>", nombre_archivo_open);
                        stream_send_empty_buffer(fs->socket_kernel, HANDSHAKE_ok_continue); // NOTIFICO A KERNEL QUE EL ARCHIVO EXISTE Y LO AGREGUE A SU TABLA GLOBAL
                    } else {
                        // CASO: NO EXISTE EL FCB DE ESE ARCHIVO. TIENE QUE SOLICITAR CREARLO
                        stream_send_empty_buffer(fs->socket_kernel, HEADER_error);
                        // ESTOY USANDO EL HEADER_ERROR PARA INDICAR QUE NO SE PUDO ABRIR EL ARCHIVO. A CONFIRMAR SI ESTO ESTA OK
                    }                                                                            
                }

                free(nombre_archivo_open);
                buffer_destroy(buffer_nombre_archivo_open);

            break;
            case HEADER_f_create:

            break;
            case HEADER_f_truncate:

                char* nombre_archivo_truncate;
                t_buffer* buffer_nombre_archivo_truncate = buffer_create();

                uint32_t tamanio_archivo_truncate;
                t_buffer* buffer_tamanio_archivo_truncate = buffer_create();

                stream_recv_buffer(fs->socket_kernel, buffer_nombre_archivo_truncate); // RECIBO EL BUFFER NOMBRE DE ARCHIVO DE KERNEL
                nombre_archivo_truncate = buffer_unpack_string(buffer_nombre_archivo_truncate); // DESERIALIZO EL BUFFER MANDADO POR KERNEL
                stream_recv_buffer(fs->socket_kernel, buffer_tamanio_archivo_truncate); // RECIBO EL BUFFER TAMANIO DE ARCHIVO DE KERNEL
                buffer_unpack(buffer_tamanio_archivo_truncate, &tamanio_archivo_truncate, sizeof(tamanio_archivo_truncate)); // DESERIALIZO EL TAMANIO DE ARCHIVO

                operacion_OK = truncar_archivo(nombre_archivo_truncate, tamanio_archivo_truncate);

                if (operacion_OK) {
                    log_info(fs->logger, "Truncar Archivo: <%s> - Tamaño: <%d>", nombre_archivo_truncate, tamanio_archivo_truncate);
                    stream_send_empty_buffer(fs->socket_kernel, HANDSHAKE_ok_continue); // NOTIFICO A KERNEL QUE EL ARCHIVO SE TRUNCO
                } else {
                    stream_send_empty_buffer(fs->socket_kernel, HEADER_error); // FALLO EN EL TRUNCATE
                    // ESTOY USANDO EL HEADER_ERROR PARA INDICAR QUE NO SE PUDO ABRIR EL ARCHIVO. A CONFIRMAR SI ESTO ESTA OK
                }  

                free(nombre_archivo_truncate);
                buffer_destroy(buffer_nombre_archivo_truncate);
                buffer_destroy(buffer_tamanio_archivo_truncate);

            break;
            case HEADER_f_read:

            break;
            case HEADER_f_write:

            break;
            default:
                log_error(fs->logger, "Peticion incorrecta.");
            break;
        }

        operacion_OK = 0;
    }
    
    printf("Hubo una desconexion");
    return;
}

int truncar_archivo(char* nombre_archivo, uint32_t nuevo_tamanio_archivo) {

    int truncado_ok;
    int pos_archivo_a_truncar = -1;
    int size_lista_fcbs = list_size(lista_fcbs);
    
    for (int i = 0; i < size_lista_fcbs; i++) {

        t_fcb* fcb_aux = malloc(sizeof(t_fcb));
        fcb_aux = list_get(lista_fcbs, i);

        if (strcmp(fcb_aux->nombre_archivo, nombre_archivo) == 0) {
            pos_archivo_a_truncar = i;
        }

        free(fcb_aux);
    }

    if (pos_archivo_a_truncar == -1) {
        truncado_ok = 0;
    } else {

        t_fcb* fcb_a_truncar = list_get(lista_fcbs, pos_archivo_a_truncar);

        if (fcb_a_truncar->tamanio_archivo > nuevo_tamanio_archivo) {
            // CASO REDUCIR EL TAMANIO DEL ARCHIVO

            fcb_a_truncar->tamanio_archivo = nuevo_tamanio_archivo;

        } else {
            // CASO AMPLIAR EL TAMANIO DEL ARCHIVO

            fcb_a_truncar->tamanio_archivo = nuevo_tamanio_archivo;

        }

        truncado_ok = 1;    
    }

    return truncado_ok;
}

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
