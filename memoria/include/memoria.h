#ifndef MEMORIA_H
#define MEMORIA_H

#include <errno.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <sys/socket.h>
#include <../../utils/src/utils/conexiones.h>
#include <../../utils/src/utils/serializacion.h>
#include <../../utils/src/utils/commons-nuestras.h>
#include <readline/readline.h>

#include <commons/log.h>
#include <commons/config.h>
#include <pthread.h>

#include <stdio.h>
#include "memoria-estructuras.h"
#include "memoria-config.h"
#include "memoria-atender-kernel.h"
#include "memoria-atender-cpu.h"
#include "memoria-atender-FS.h"

#define MEMORIA_LOG_UBICACION "logs/memoria.log"
#define MEMORIA_PROCESS_NAME "Memoria"

void aceptar_conexiones_memoria(const int );
void recibir_conexion(int );


#endif