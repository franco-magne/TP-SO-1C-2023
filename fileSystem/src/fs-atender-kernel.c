#include "../include/fs-atender-kernel.h"

void atender_kernel(t_filesystem* fs) {

    int cliente_socket = fs->socket_kernel;

    t_header header;
    while (cliente_socket != -1) {
        
        if (recv(cliente_socket, &header, sizeof(t_header), 0) == 0) {
        	printf("ERROR en la comunicacion.");
            return;
        }

        switch(header) {
            case HEADER_abrir_archivo:

            break;
            case HEADER_crear_archivo:

            break;
            case HEADER_truncar_archivo:

            break;
            case HEADER_leer_archivo:

            break;
            case HEADER_escribir_archivo:

            break;
            default:

            break;
        }

    }
    
    printf("Hubo una desconexion");
    return;
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

