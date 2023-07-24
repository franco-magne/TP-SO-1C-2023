#ifndef MEMORIA_ATENDER_CPU_H
#define MEMORIA_ATENDER_CPU_H

#include <errno.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <sys/socket.h>
#include <../../utils/src/utils/conexiones.h>
#include <../../utils/src/utils/serializacion.h>
#include <commons/log.h>
#include <commons/config.h>
#include <pthread.h>

#include <stdio.h>
#include "memoria-estructuras.h"
#include "memoria.h"


#define MEMORIA_LOG_UBICACION "logs/memoria.log"
#define MEMORIA_PROCESS_NAME "Memoria"

void atender_peticiones_cpu(int socketCpu);

#endif