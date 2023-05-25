#ifndef CPU_CONEXIONES_H
#define CPU_CONEXIONES_H


///////////////////////// BIBLOTECAS BASICAS ///////////////////
#include <sys/socket.h>

//////////////////////// BIBLOTECA DE COMMONS //////////////////


///////////////////////// BIBLOTECAS NUESTRAS //////////////////
#include "cpu-estructuras.h"
#include <../../utils/src/utils/conexiones.h>

void aceptar_conexion_dispatch(const int socketEscuchaDispatch, struct sockaddr* cliente, socklen_t* len);
int inicializar_servidor_cpu_dispatch(t_cpu_config* cpuConfig, t_log* cpuLogger);


#endif