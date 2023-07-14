#include "../include/consola_config.h"


t_consola_config *configDeKernel;

void inicializar_config(t_config* tempCfg)
{
    configDeKernel = malloc(sizeof(t_consola_config));
    configDeKernel->IP_KERNEL = strdup(config_get_string_value(tempCfg, "IP"));
    configDeKernel->PUERTO_KERNEL = strdup(config_get_string_value(tempCfg, "PUERTO"));
}

char* consola_config_get_kernel_ip(t_consola_config* self) {
    return self->IP_KERNEL;
}

char* consola_config_get_kernel_port(t_consola_config* self) {
    return self->PUERTO_KERNEL;
}

void consola_config_destroy(t_consola_config* self) {
    free(self->IP_KERNEL);
    free(self->PUERTO_KERNEL);
    free(self);
}
