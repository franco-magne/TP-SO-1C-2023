#include <../include/memoria-config.h>


t_memoria_config* memoria_config_initializer(t_config* tempCfg) 
{
    t_memoria_config* memoriaConfig = malloc(sizeof(*memoriaConfig));

    memoriaConfig->IP_ESCUCHA = strdup(config_get_string_value(tempCfg, "IP_MEMORIA"));
    memoriaConfig->PUERTO_ESCUCHA = strdup(config_get_string_value(tempCfg, "PUERTO_MEMORIA"));
    memoriaConfig->TAM_MEMORIA = (uint32_t) config_get_int_value(tempCfg, "TAM_MEMORIA");
    memoriaConfig->TAM_SEGMENTO_0 = (uint32_t) config_get_int_value(tempCfg, "TAM_SEGMENTO_0");
    memoriaConfig->CANT_SEGMENTOS = (uint32_t) config_get_int_value(tempCfg, "CANT_SEGMENTOS");
    memoriaConfig->RETARDO_MEMORIA = (uint32_t) config_get_int_value(tempCfg, "RETARDO_MEMORIA");
    memoriaConfig->RETARDO_COMPACTACION = (uint32_t) config_get_int_value(tempCfg, "RETARDO_COMPACTACION");
    memoriaConfig->ALGORITMO_ASIGNACION = strdup(config_get_string_value(tempCfg, "ALGORITMO_ASIGNACION"));

    return memoriaConfig;
}




void memoria_config_destroy(t_memoria_config* self) 
{
    free(self->IP_ESCUCHA);
    free(self->PUERTO_ESCUCHA);
    free(self);
}

char* memoria_config_get_ip_escucha(t_memoria_config* self)
{
    return self->IP_ESCUCHA;
}

char* memoria_config_get_puerto_escucha(t_memoria_config* self) 
{
    return self->PUERTO_ESCUCHA;
}

uint32_t memoria_config_get_tamanio_memoria(t_memoria_config* self) 
{
    return self->TAM_MEMORIA;
}

uint32_t memoria_config_get_tamanio_segmento_0(t_memoria_config* self) 
{
    return self->TAM_SEGMENTO_0;
}

uint32_t memoria_config_get_cant_de_segmentos(t_memoria_config* self) 
{
    return self->CANT_SEGMENTOS;
}

uint32_t memoria_config_get_retardo_compactacion(t_memoria_config* self) 
{
    return self->RETARDO_COMPACTACION;
}

uint32_t memoria_config_get_retardo_memoria(t_memoria_config* self) 
{
    return self->RETARDO_MEMORIA;
}

uint32_t memoria_config_get_procesos_totales(t_memoria_config* self) 
{
    //return self->TAM_MEMORIA / (self->TAM_PAGINA * self->MARCOS_POR_PROCESO);
    return 0;
}