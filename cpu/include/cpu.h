#ifndef CPU_H
#define CPU_H

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <sys/socket.h>
#include <../../utils/src/utils/conexiones.h>
#include <commons/log.h>
#include <commons/config.h>
#include <pthread.h>

#include "cpu-atender-kernel.h"
#include "cpu-estructuras.h"
#include "cpu-conexiones.h"

#define CPU_LOG_UBICACION "logs/cpu.log"
#define CPU_PROCESS_NAME "Cpu"

#endif