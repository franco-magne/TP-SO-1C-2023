#ifndef KERNEL_H
#define KERNEL_H

///////////////////////////////// BIBLOTECAS BASICAS /////////////////
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <sys/socket.h>
#include <pthread.h>
#include <time.h>

///////////////////////////////// BIBLOTECAS COMMONS /////////////////
#include <commons/log.h>
#include <commons/config.h>

//////////////////////////////// BIBLOTECA UTILS NUESTRA //////////////
#include "kernel-estado.h"
#include "kernel-config.h"
#include "pcb.h"
#include <utils/conexiones.h>
#include "utils/serializacion.h"
#include "../../utils/src/utils/instrucciones.h"
#include "kernel-cpu-adapter.h"
#include "kernel-conexiones.h"
#include "kernel-algoritmos.h"
#include "../../utils/src/utils/commons-nuestras.h"
#include "kernel-instrucciones.h"
#include "kernel-memoria-adapter.h"





#define KERNEL_CONFIG_UBICACION "config/kernel.config"
#define KERNEL_LOG_UBICACION "logs/kernel.log"
#define KERNEL_PROCESS_NAME "Kernel"



#endif