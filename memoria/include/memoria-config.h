#ifndef MEMORIA_CONFIG_H
#define MEMORIA_CONFIG_H

//Standard libraries
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
//Commons libraries
#include <commons/log.h>
#include <commons/config.h>
//Bibliotecas static-utils
//Bibliotecas internas modulo consola

typedef struct  {

    char* IP_ESCUCHA;   //seria IP_MEMORIA
    char* PUERTO_ESCUCHA;   //seria PUERTO_MEMORIA
    uint32_t TAM_MEMORIA;
    uint32_t TAM_SEGMENTO_0;
    uint32_t CANT_SEGMENTOS;
    uint32_t RETARDO_MEMORIA;
    uint32_t RETARDO_COMPACTACION;
    char* ALGORITMO_ASIGNACION;
    int SOCKET_KERNEL;

} t_memoria_config;

/*
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

*/

t_memoria_config* memoria_config_initializer(t_config* );
void memoria_config_destroy(t_memoria_config* );
char* memoria_config_get_ip_escucha(t_memoria_config*);
char* memoria_config_get_puerto_escucha(t_memoria_config*);
char* memoria_config_get_algoritmo_asignacion(t_memoria_config*);
uint32_t memoria_config_get_cant_de_segmentos(t_memoria_config*);
uint32_t memoria_config_get_procesos_totales(t_memoria_config*);
uint32_t memoria_config_get_retardo_memoria(t_memoria_config*);
uint32_t memoria_config_get_retardo_compactacion(t_memoria_config*);
uint32_t memoria_config_get_tamanio_memoria(t_memoria_config*);
uint32_t memoria_config_get_tamanio_segmento_0(t_memoria_config*);
int memoria_config_get_socket_kernel(t_memoria_config* this);
void memoria_config_set_socket_kernel(t_memoria_config* this, int socketKernel);


#endif