#include <errno.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <sys/socket.h>

//include "common_flags.h"
//#include "connections.h"
//#include "kernel_config.h"
//#include "scheduler.h"
//#include "stream.h"
//#include <stdio.h>
int main() {
   // printf() displays the string inside quotation

   int socketEscucha = iniciar_servidor("127.0.0.1" , "8000");
    if (socketEscucha == -1) {
       // log_error(kernelLogger, "Error al intentar iniciar servidor");
       // __kernel_destroy(kernelConfig, kernelLogger);
       printf("No pudo contectarse consola");
       return -1;
        // exit(-1);
    }

   struct sockaddr cliente = {0};
    socklen_t len = sizeof(cliente);
   // log_info(kernelLogger, "A la escucha de nuevas conexiones en puerto %d", socketEscucha);
    for (;;) {
        int clienteAceptado = accept(socketEscucha, &cliente, &len);
        if (clienteAceptado > -1) {
            int* socketCliente = malloc(sizeof(*socketCliente));
            *socketCliente = clienteAceptado;
            printf("Cliente aceptado");
            //__crear_hilo_handler_conexion_entrante(socketCliente);
        } else {
            //log_error(kernelLogger, "Error al aceptar conexión: %s", strerror(errno));
            printf("No pudo conectarse al cliente");
             return 0;
        }
    }


   return 0;
}
