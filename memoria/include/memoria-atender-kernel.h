#ifndef MEMORIA_ATENDER_KERNEL_H
#define MEMORIA_ATENDER_KERNEL_H

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
#include "memoria-administracion.h"

void atender_peticiones_kernel(int socketKernel);
bool puedo_crear_segmento_o_proceso(uint32_t tamanio);

#endif