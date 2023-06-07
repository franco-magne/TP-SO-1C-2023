#ifndef FILESYSTEM_H
#define FILESYSTEM_H

///////////////////////////////// BIBLOTECAS BASICAS /////////////////
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <sys/socket.h>
#include <pthread.h>

//////////////////////////////// BIBLOTECA UTILS NUESTRA //////////////
#include <../../utils/src/utils/conexiones.h>
#include "../include/fs-structures.h"

///////////////////////////////// BIBLOTECAS COMMONS /////////////////
#include <commons/log.h>
#include <commons/config.h>

#define FS_CONFIG_UBICACION "config/fileSystem.config"
#define FS_LOG_UBICACION "logs/fileSystem.log"
#define FS_PROCESS_NAME "FILESYSTEM" 



#endif