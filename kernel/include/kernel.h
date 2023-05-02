#ifndef KERNEL_H
#define KERNEL_H

#include <errno.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <sys/socket.h>
#include <utils/conexiones.h>
#include "utils/serializacion.h"
#include <commons/log.h>
#include <commons/config.h>
#include <pthread.h>

//include "common_flags.h"
//#include "connections.h"
//#include "kernel_config.h"
//#include "scheduler.h"
//#include "stream.h"
#include <stdio.h>

#define KERNEL_CONFIG_UBICACION "config/kernel.config"
#define KERNEL_LOG_UBICACION "logs/kernel.log"
#define KERNEL_PROCESS_NAME "Kernel"



#endif