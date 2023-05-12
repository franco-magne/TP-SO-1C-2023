#include <../include/cpu-estructuras.h>


void cpu_config_initializer(void* moduleConfig, t_config* tempCfg) 
{
    t_cpu_config* cpuConfig = (t_cpu_config*)moduleConfig;
    
    cpuConfig->RETARDO_INSTRUCCION= config_get_int_value(tempCfg, "RETARDO_INSTRUCCION");
    cpuConfig->IP_MEMORIA = strdup(config_get_string_value(tempCfg, "IP_MEMORIA"));
    cpuConfig->PUERTO_MEMORIA = strdup(config_get_string_value(tempCfg, "PUERTO_MEMORIA"));
    cpuConfig->IP_ESCUCHA = strdup(config_get_string_value(tempCfg, "IP_ESCUCHA"));
    cpuConfig->PUERTO_ESCUCHA_DISPATCH = strdup(config_get_string_value(tempCfg, "PUERTO_ESCUCHA"));
    cpuConfig->SOCKET_MEMORIA = -1;
    cpuConfig->SOCKET_DISPATCH_CPU = -1;
    cpuConfig->SOCKET_INTERRUPT_CPU = -1;
}
void cpu_config_destroy(t_cpu_config* self) 
{
    free(self->IP_MEMORIA);
    free(self->PUERTO_MEMORIA);
    free(self->IP_ESCUCHA);
    free(self->PUERTO_ESCUCHA_DISPATCH);
    free(self);
}

t_cpu_config* cpu_config_create(char* cpuConfigPath, t_log* cpuLogger) 
{
    t_cpu_config* self = malloc(sizeof(*self));
    config_init(self, cpuConfigPath, cpuLogger, cpu_config_initializer);
    return self;
}



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
void cpu_config_set_socket_memoria(t_cpu_config* self, int socketMemoria) 
{
    self->SOCKET_MEMORIA = socketMemoria;
}
void cpu_config_set_socket_dispatch(t_cpu_config* self, int socketDispatch) 
{
    self->SOCKET_DISPATCH_CPU = socketDispatch;
}
