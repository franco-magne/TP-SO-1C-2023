#ifndef CONSOLA_H
#define CONSOLA_H

#include <stdio.h>
#include "../../utils/src/utils/conexiones.h"
#include <commons/log.h>
#include <commons/config.h>

typedef struct{
    char* IP_KERNEL;
    char* PUERTO_KERNEL;
}t_consola_config;


static void inicializar_config(t_config* tempCfg);
char* consola_config_get_kernel_ip(t_consola_config* self);
char* consola_config_get_kernel_port(t_consola_config* self);
void consola_config_destroy(t_consola_config* self);



#endif