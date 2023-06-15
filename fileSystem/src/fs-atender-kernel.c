#include "../include/fs-atender-kernel.h"

t_list* lista_fcbs;

void atender_kernel(t_filesystem* fs) {

    int operacion_OK = 0;
    lista_fcbs = crear_fcbs(fs->logger);
    
    while (fs->socket_kernel != -1) {

        uint8_t header = stream_recv_header(fs->socket_kernel); // RECIBO LA OPERACION QUE KERNEL QUIERA SOLICITAR

        switch(header) {
            case HEADER_f_open:
                                        
                char* nombre_archivo_open;
                t_buffer* buffer_nombre_archivo_open = buffer_create();

                stream_recv_buffer(fs->socket_kernel, buffer_nombre_archivo_open); // RECIBO EL BUFFER NOMBRE DE ARCHIVO DE KERNEL
                nombre_archivo_open = buffer_unpack_string(buffer_nombre_archivo_open); // DESERIALIZO EL BUFFER MANDADO POR KERNEL

                operacion_OK = abrir_archivo(nombre_archivo_open);

                if (operacion_OK) {
                    log_info(fs->logger, "Abrir archivo: <%s>", nombre_archivo_open);
                    stream_send_empty_buffer(fs->socket_kernel, HANDSHAKE_ok_continue); // NOTIFICO A KERNEL QUE EL ARCHIVO EXISTE Y LO AGREGUE A SU TABLA GLOBAL
                } else {
                    stream_send_empty_buffer(fs->socket_kernel, HEADER_error); // NO EXISTE EL FCB DE ESE ARCHIVO. TIENE QUE SOLICITAR CREARLO
                    // ESTOY USANDO EL HEADER_ERROR PARA INDICAR QUE NO SE PUDO ABRIR EL ARCHIVO. A CONFIRMAR SI ESTO ESTA OK
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

            break;
        }

        operacion_OK = 0;
    }
    
    printf("Hubo una desconexion");
    return;
}

/*
t_buffer* bufferArchivo = buffer_create();
char* nombreArchivo;
int ejemplo;
buffer_pack_string(bufferArchivo, nombreArchivo); // EMPAQUETADO STRING
buffer_pack(bufferArchivo,&ejemplo,sizeof(ejemplo)); // EMpaquetado int

stream_send_buffer(kernel_config_get_socket_filesystem(kernelConfig),HEADER_f_close,bufferArchivo ); // ENVIAR

buffer_destroy(bufferArchivo);



////////////////////// FILE SYSTEM ///////////////

uint8_t header = stream_recv_header(socketKernel); // PRIMERO RECIBO HEADER
if(header == HEADER_f_close){
    char* nombreARchivoACerrar;
    int ejemplo;
    t_buffer* bufferArchivoCerrar = buffer_create();
    stream_recv_buffer(socketKernel, bufferArchivoCerrar); //DESPUES RECIBO EL BUFFER
    nombreARchivoACerrar= buffer_unpack_string(bufferArchivoCerrar); // ESTE USA PARA STRING
    buffer_unpack(bufferArchivoCerrar, &ejemplo, sizeof(ejemplo)); // ESTE LO USAS PARA INT Y ESO
    log_info(fileSystemlog, "nombre a archivo a cerrar <%s> ", nombreARchivoACerrar);

    stream_send_empty_buffer(socketKernel, HANDSHAKE_ok_continue); // ACA ME NOTIFICAS A MI QUE YA LO CERRASTE
}

*/

int truncar_archivo(char* nombre_archivo, uint32_t tamanio_archivo) {

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
        t_fcb* fcb_a_truncar = malloc(sizeof(t_fcb));
        t_fcb* nuevo_fcb = malloc(sizeof(t_fcb));

        fcb_a_truncar = list_get(lista_fcbs, pos_archivo_a_truncar);
        nuevo_fcb->nombre_archivo = fcb_a_truncar->nombre_archivo;
        nuevo_fcb->puntero_directo = fcb_a_truncar->puntero_directo;
        nuevo_fcb->puntero_indirecto = fcb_a_truncar->puntero_indirecto;

        if (fcb_a_truncar->tamanio_archivo > tamanio_archivo) {
            // CASO REDUCIR EL TAMANIO DEL ARCHIVO

            nuevo_fcb->tamanio_archivo = fcb_a_truncar->tamanio_archivo;
            list_replace(lista_fcbs, pos_archivo_a_truncar, nuevo_fcb);

        } else {
            // CASO AMPLIAR EL TAMANIO DEL ARCHIVO

            nuevo_fcb->tamanio_archivo = fcb_a_truncar->tamanio_archivo;
            list_replace(lista_fcbs, pos_archivo_a_truncar, nuevo_fcb);

        }

        truncado_ok = 1;
        free(nuevo_fcb);
        free(fcb_a_truncar);        
    }

    return truncado_ok;
}

int abrir_archivo(char* nombre_archivo) {

    int encontrado = 0;
    int size_lista_fcbs = list_size(lista_fcbs);
    
    for (int i = 0; i < size_lista_fcbs; i++) {

        t_fcb* fcb_aux = malloc(sizeof(t_fcb));
        fcb_aux = list_get(lista_fcbs, i);

        if (strcmp(fcb_aux->nombre_archivo, nombre_archivo) == 0) {
            encontrado = 1;
        }

        free(fcb_aux);
    }

    return encontrado;
}

int fs_escuchando_en(int server_fs, t_filesystem* fs) {

    int socket_kernel = esperar_cliente(server_fs);
    fs->socket_kernel = socket_kernel;

    if (socket_kernel != -1) {
        
        pthread_t hilo;
        pthread_create(&hilo, NULL, (void*) atender_kernel, (void*) fs);
        pthread_detach(hilo);
        
        return 1;
    }

    return 0;
}
