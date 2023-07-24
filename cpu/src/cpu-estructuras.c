#include <../include/cpu-estructuras.h>


t_cpu_config* cpu_config_initializer(t_config* tempCfg) 
{
    t_cpu_config* self = malloc(sizeof(*self));

    self->RETARDO_INSTRUCCION= config_get_int_value(tempCfg, "RETARDO_INSTRUCCION");
    self->IP_MEMORIA = strdup(config_get_string_value(tempCfg, "IP_MEMORIA"));
    self->PUERTO_MEMORIA = strdup(config_get_string_value(tempCfg, "PUERTO_MEMORIA"));
    self->IP_ESCUCHA = strdup(config_get_string_value(tempCfg, "IP_ESCUCHA"));
    self->PUERTO_ESCUCHA_DISPATCH = strdup(config_get_string_value(tempCfg, "PUERTO_ESCUCHA"));
    self-> TAMANIO_MAXIMO_SEGMENTO = config_get_int_value(tempCfg, "TAM_MAX_SEGMENTO");
    self->SOCKET_MEMORIA = -1;
    self->SOCKET_DISPATCH_CPU = -1;
    
    return self;
}

void cpu_config_destroy(t_cpu_config* self) 
{
    free(self->IP_MEMORIA);
    free(self->PUERTO_MEMORIA);
    free(self->IP_ESCUCHA);
    free(self->PUERTO_ESCUCHA_DISPATCH);
    free(self);
}


///////////////////////////// GETTERS ////////////////////////////////////

uint32_t cpu_config_get_retardo_instruccion(t_cpu_config* self) 
{
    return self->RETARDO_INSTRUCCION;
}

char* cpu_config_get_ip_memoria(t_cpu_config* self)
{
    return self->IP_MEMORIA;
}

char* cpu_config_get_puerto_memoria(t_cpu_config* self) 
{
    return self->PUERTO_MEMORIA;
}

char* cpu_config_get_puerto_dispatch(t_cpu_config* self) 
{
    return self->PUERTO_ESCUCHA_DISPATCH;
}


char* cpu_config_get_ip_cpu(t_cpu_config* self) 
{
    return self->IP_ESCUCHA;
}

int cpu_config_get_socket_dispatch(t_cpu_config* self) 
{
    return self->SOCKET_DISPATCH_CPU;
}

int cpu_config_get_socket_memoria(t_cpu_config* self)
{
    return self->SOCKET_MEMORIA;
}

int cpu_config_get_tamanio_maximo_segmento(t_cpu_config* self)
{
    return self->TAMANIO_MAXIMO_SEGMENTO;
}


///////////////////////////// SETTERS ////////////////////////////////////

void cpu_config_set_socket_memoria(t_cpu_config* self, int socketMemoria) 
{
    self->SOCKET_MEMORIA = socketMemoria;
}

void cpu_config_set_socket_dispatch(t_cpu_config* self, int socketDispatch) 
{
    self->SOCKET_DISPATCH_CPU = socketDispatch;
}

void cpu_set_recurso_sem(recurso* this, char* recurso)
{
    this->recursoUtilizado = recurso;
}

void cpu_set_recursoIO(recurso* this, uint32_t tiempoIO)
{
    this->tiempoIO = tiempoIO;
}

char* cpu_get_recurso_sem(recurso* this)
{
    return this->recursoUtilizado;
}

uint32_t cpu_get_recurso_IO(recurso* this)
{
    return this->tiempoIO;
}
