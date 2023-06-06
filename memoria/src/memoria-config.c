#include <../include/memoria-config.h>


t_memoria_config* memoria_config_initializer(t_config* tempCfg) 
{
    t_memoria_config* memoriaConfig = malloc(sizeof(*memoriaConfig));
    
    memoriaConfig->IP_ESCUCHA = strdup(config_get_string_value(tempCfg, "IP_ESCUCHA"));
    memoriaConfig->PUERTO_ESCUCHA = strdup(config_get_string_value(tempCfg, "PUERTO_ESCUCHA"));
    memoriaConfig->TAM_MEMORIA = (uint32_t) config_get_int_value(tempCfg, "TAM_MEMORIA");
    memoriaConfig->TAM_SEGMENTO = (uint32_t) config_get_int_value(tempCfg, "TAM_SEGMENTO");
    memoriaConfig->CANT_SEGMENTOS = (uint32_t) config_get_int_value(tempCfg, "CANT_SEGMENTOS");
    memoriaConfig->RETARDO_MEMORIA = (uint32_t) config_get_int_value(tempCfg, "RETARDO_MEMORIA");
    memoriaConfig->RETARDO_COMPACTACION = (uint32_t) config_get_int_value(tempCfg, "RETARDO_COMPACTACION");
    memoriaConfig->ALGORITMO_ASIGNACION = strdup(config_get_string_value(tempCfg, "ALGORITMO_ASIGNACION"));

    return memoriaConfig;
}


t_memoria_config* memoria_config_create(char* memoriaConfigPath, t_log* memoriaLogger)
{
    t_memoria_config* this = malloc(sizeof(*this));
    config_init(this, memoriaConfigPath, memoriaLogger, memoria_config_initializer);
    return this;
}

void memoria_config_destroy(t_memoria_config* this) 
{
    free(this->IP_ESCUCHA);
    free(this->PUERTO_ESCUCHA);
    free(this);
}

char* memoria_config_get_ip_escucha(t_memoria_config* this)
{
    return this->IP_ESCUCHA;
}

char* memoria_config_get_puerto_escucha(t_memoria_config* this) 
{
    return this->PUERTO_ESCUCHA;
}

uint32_t memoria_config_get_tamanio_memoria(t_memoria_config* this) 
{
    return this->TAM_MEMORIA;
}

uint32_t memoria_config_get_tamanio_segmento(t_memoria_config* this) 
{
    return this->TAM_SEGMENTO;
}

uint32_t memoria_config_get_cant_de_segmentos(t_memoria_config* this) 
{
    return this->CANT_SEGMENTOS;
}

uint32_t memoria_config_get_retardo_compactacion(t_memoria_config* this) 
{
    return this->RETARDO_COMPACTACION;
}

uint32_t memoria_config_get_retardo_memoria(t_memoria_config* this) 
{
    return this->RETARDO_MEMORIA;
}

uint32_t memoria_config_get_procesos_totales(t_memoria_config* this) 
{
    //return this->TAM_MEMORIA / (this->TAM_PAGINA * this->MARCOS_POR_PROCESO);
    return 0;
}