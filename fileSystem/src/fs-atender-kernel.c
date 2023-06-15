#include "../include/fs-atender-kernel.h"

t_list* lista_fcbs;

void atender_kernel(t_filesystem* fs) {

    lista_fcbs = crear_fcbs(fs->logger);
    
    while (fs->socket_kernel != -1) {

        uint8_t header = stream_recv_header(fs->socket_kernel); // RECIBO LA OPERACION QUE KERNEL QUIERA SOLICITAR

        switch(header) {
            case HEADER_f_open:
                    
                    int retorno = 0;
                    char* nombre_archivo;
                    t_buffer* buffer_nombre_archivo = buffer_create();

                    stream_recv_buffer(fs->socket_kernel, buffer_nombre_archivo); // RECIBO EL BUFFER NOMBRE DE ARCHIVO DE KERNEL
                    nombre_archivo = buffer_unpack_string(buffer_nombre_archivo); // DESERIALIZO EL BUFFER MANDADO POR KERNEL

                    retorno = abrir_archivo(nombre_archivo);

                    if (retorno) {
                        log_info(fs->logger, "Abrir archivo: <%s>", nombre_archivo);
                        stream_send_empty_buffer(fs->socket_kernel, HANDSHAKE_ok_continue); // NOTIFICO A KERNEL QUE EL ARCHIVO EXISTE Y LO AGREGUE A SU TABLA GLOBAL
                    } else {
                        stream_send_empty_buffer(fs->socket_kernel, HEADER_error); // NO EXISTE EL FCB DE ESE ARCHIVO. TIENE QUE SOLICITAR CREARLO
                        // ESTOY USANDO EL HEADER_ERROR PARA INDICAR QUE NO SE PUDO ABRIR EL ARCHIVO. A CONFIRMAR SI ESTO ESTA OK
                    }         

            break;
            case HEADER_f_create:

            break;
            case HEADER_f_truncate:

            break;
            case HEADER_f_read:

            break;
            case HEADER_f_write:

            break;
            default:

            break;
        }

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
