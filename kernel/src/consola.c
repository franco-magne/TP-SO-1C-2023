#include <errno.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <sys/socket.h>
#include "../../utils/src/utils/conexiones.h"


//include "common_flags.h"
//#include "connections.h"
//#include "kernel_config.h"
//#include "scheduler.h"
//#include "stream.h"
#include <stdio.h>


int iniciar_servidor(char* ip, char* port)
{

	int socket_servidor;

	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo("127.0.0.1", "8000", &hints, &servinfo);

	socket_servidor = socket(servinfo->ai_family,
                    servinfo->ai_socktype,
                    servinfo->ai_protocol);


    bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);

    listen(socket_servidor, SOMAXCONN);

	freeaddrinfo(servinfo);
	printf("Listo para escuchar a mi cliente");

	return socket_servidor;
}

int esperar_cliente(int socket_servidor)
{


	// Aceptamos un nuevo cliente
	int socket_cliente;
    socket_cliente = accept(socket_servidor, NULL, NULL);
	printf("Se conecto un cliente!");

	return socket_cliente;
}



int main() {
   // printf() displays the string inside quotation

    int server_fd = iniciar_servidor("8000", "127");
	printf("Servidor listo para recibir al cliente");
	int cliente_fd = esperar_cliente(server_fd);


   return 0;
}



