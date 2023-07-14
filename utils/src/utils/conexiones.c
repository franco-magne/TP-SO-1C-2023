#include "conexiones.h"


int iniciar_servidor(char* ip, char* port) {

    int optVal = 1;
    struct addrinfo hints;
    struct addrinfo* serverInfo;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int rv = getaddrinfo(ip, port, &hints, &serverInfo);
    if (rv != 0) {
        printf("getaddrinfo error: %s\n", gai_strerror(rv));
        return EXIT_FAILURE;
    }

    int serverSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
    if (-1 == serverSocket) {
        printf("Socket creation error\n%s", strerror(errno));
        return EXIT_FAILURE;
    }

    int sockOpt = setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof(optVal));
    if (-1 == sockOpt) {
        printf("Sockopt error\n%s", strerror(errno));
        return EXIT_FAILURE;
    }

    int bindVal = bind(serverSocket, serverInfo->ai_addr, serverInfo->ai_addrlen);
    if (-1 == bindVal) {
        printf("Mismatched bind\n%s", strerror(errno));
        return EXIT_FAILURE;
    }

    int listenVal = listen(serverSocket, SOMAXCONN);
    if (-1 == listenVal) {
        printf("Listen error\n%s", strerror(errno));
        return EXIT_FAILURE;
    }

    freeaddrinfo(serverInfo);

    return serverSocket;
}

int conectar_a_servidor(char* ip, char* port) {

    int conn;
    struct addrinfo hints;
    struct addrinfo* serverInfo;
    struct addrinfo* p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int rv = getaddrinfo(ip, port, &hints, &serverInfo);
    if (rv != 0) {
        fprintf(stderr, "getaddrinfo error: %s", gai_strerror(rv));
        return EXIT_FAILURE;
    }

    for (p = serverInfo; p != NULL; p = p->ai_next) {
        conn = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (-1 == conn) {
            continue;
        }
        if (connect(conn, p->ai_addr, p->ai_addrlen) != -1) {
            break;
        }
        close(conn);
    }

    freeaddrinfo(serverInfo);
    if (conn != -1 && p != NULL) {
        return conn;
    }

    return -1;
}

int esperar_cliente(int socket_servidor)
{
	// Aceptamos un nuevo cliente
	int socket_cliente;
    socket_cliente = accept(socket_servidor, NULL, NULL);

	return socket_cliente;
}

char* obtener_ip() {
    int fd;
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Error al crear el socket");
        return NULL;
    }

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(80); // Puerto arbitrario

    if (connect(fd, (struct sockaddr*)&sin, len) == -1) {
        perror("Error al conectar");
        close(fd);
        return NULL;
    }

    if (getsockname(fd, (struct sockaddr*)&sin, &len) == -1) {
        perror("Error al obtener la dirección IP");
        close(fd);
        return NULL;
    }

    close(fd);

    char* ip = malloc(INET_ADDRSTRLEN);
    if (ip == NULL) {
        perror("Error al asignar memoria para la IP");
        return NULL;
    }

    if (inet_ntop(AF_INET, &(sin.sin_addr), ip, INET_ADDRSTRLEN) == NULL) {
        perror("Error al convertir la dirección IP");
        free(ip);
        return NULL;
    }

    return ip;
}
