#ifndef MEMORIA_CONFIG_H
#define MEMORIA_CONFIG_H

//Standard libraries
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
//Commons libraries
#include <commons/log.h>
//Bibliotecas static-utils
//Bibliotecas internas modulo consola
#include <memoria-estructuras.h> 

int config_init(void* moduleConfig, char* pathToConfig, t_log* moduleLogger,
                void (*config_initializer)(void* moduleConfig, t_config* tempConfig)) {
    t_config* tempConfig = config_create(pathToConfig);
    if (NULL == tempConfig) {
        log_error(moduleLogger, "Path del archivo de configuracion \"%s\" no encontrado", pathToConfig);
        return -1;
    }
    config_initializer(moduleConfig, tempConfig);
    log_info(moduleLogger, "Inicialización de campos de configuracion correcta");
    config_destroy(tempConfig);
    return 1;
}

void memoria_config_initializer(void* moduleConfig, t_config* tempCfg);
t_memoria_config* memoria_config_create(char* memoriaConfigPath, t_log*);
void memoria_config_destroy(t_memoria_config* self);
char* memoria_config_get_ip_escucha(t_memoria_config*);
char* memoria_config_get_puerto_escucha(t_memoria_config*);
uint32_t memoria_config_get_cant_de_segmentos(t_memoria_config*);
uint32_t memoria_config_get_procesos_totales(t_memoria_config*);
uint32_t memoria_config_get_retardo_memoria(t_memoria_config*);
uint32_t memoria_config_get_retardo_compactacion(t_memoria_config*);
uint32_t memoria_config_get_tamanio_memoria(t_memoria_config*);
uint32_t memoria_config_get_tamanio_segmento(t_memoria_config*);


#endif