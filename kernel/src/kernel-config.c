#include <../include/kernel-config.h>

t_kernel_config* kernel_config_initializer(t_config* tempCfg)
{
    t_kernel_config* kernelConfig = malloc(sizeof(*kernelConfig));
    
    kernelConfig->IP_MEMORIA = strdup(config_get_string_value(tempCfg, "IP_MEMORIA"));
    kernelConfig->PUERTO_MEMORIA = strdup(config_get_string_value(tempCfg, "PUERTO_MEMORIA"));
    kernelConfig->IP_FILESYSTEM = strdup(config_get_string_value(tempCfg, "IP_FILESYSTEM"));
    kernelConfig->PUERTO_FILESYSTEM = strdup(config_get_string_value(tempCfg, "PUERTO_FILESYSTEM"));
    kernelConfig->IP_CPU = strdup(config_get_string_value(tempCfg, "IP_CPU"));
    kernelConfig->PUERTO_CPU = strdup(config_get_string_value(tempCfg, "PUERTO_CPU"));
    kernelConfig->IP_ESCUCHA = strdup(config_get_string_value(tempCfg, "IP_ESCUCHA"));
    kernelConfig->PUERTO_ESCUCHA = strdup(config_get_string_value(tempCfg, "PUERTO_ESCUCHA"));
    kernelConfig->ALGORITMO_PLANIFICACION = strdup(config_get_string_value(tempCfg, "ALGORITMO_PLANIFICACION"));
    kernelConfig->ESTIMACION_INICIAL = config_get_int_value(tempCfg, "ESTIMACION_INICIAL");
    kernelConfig->HRRN_ALFA =  config_get_int_value(tempCfg, "HRRN_ALFA");
   // kernelConfig->SOCKET_MEMORIA = -1;
   // kernelConfig->SOCKET_DISPATCH_CPU = -1;
   // kernelConfig->SOCKET_INTERRUPT_CPU = -1;
    kernelConfig->GRADO_MULTIPROGRAMACION = config_get_int_value(tempCfg, "GRADO_MAX_MULTIPROGRAMACION");
    kernelConfig->RECURSOS = config_get_array_value(tempCfg, "RECURSOS");
    kernelConfig->INSTANCIAS_RECURSOS = config_get_array_value(tempCfg, "INSTANCIAS_RECURSOS");
    
    return kernelConfig;
}

t_kernel_config* kernel_config_create(char* kernelConfigPath, t_log* logger)
{
    t_kernel_config* this = malloc(sizeof(*this));
    log_info(logger,"hola");
   // config_init(this, kernelConfigPath, logger, kernel_config_initializer);
    return this;
}

char* kernel_config_get_ip_memoria(t_kernel_config* this) 
{
    return this->IP_MEMORIA;
}

char* kernel_config_get_puerto_memoria(t_kernel_config* this) 
{
    return this->PUERTO_MEMORIA;
}

char* kernel_config_get_ip_file_system(t_kernel_config* this) 
{
    return this->IP_FILESYSTEM;
}

char* kernel_config_get_puerto_file_system(t_kernel_config* this) 
{
    return this->PUERTO_FILESYSTEM;
}

char* kernel_config_get_ip_cpu(t_kernel_config* this) 
{
    return this->IP_CPU;
}

char* kernel_config_get_puerto_cpu(t_kernel_config* this) 
{
    return this->PUERTO_CPU;
}

char* kernel_config_get_ip_escucha(t_kernel_config* this) 
{
    return this->IP_ESCUCHA;
}

char* kernel_config_get_puerto_escucha(t_kernel_config* this) 
{
    return this->PUERTO_ESCUCHA;
}


int kernel_config_get_grado_multiprogramacion(t_kernel_config* this) 
{
    return this->GRADO_MULTIPROGRAMACION;
}


bool kernel_config_es_algoritmo_fifo(t_kernel_config* this) 
{
    return strcmp(this->ALGORITMO_PLANIFICACION, "FIFO") == 0;
}

bool kernel_config_es_algoritmo_HRRN(t_kernel_config* this) 
{
    return strcmp(this->ALGORITMO_PLANIFICACION, "HRRN") == 0;
}


char* kernel_config_get_algoritmo_planificacion(t_kernel_config* this)
{
    return this->ALGORITMO_PLANIFICACION;
}

// FALTAN ESTIMACION INICIAL
// FALTAN HRRN_ALFA
// FALTA RECURSOS
// FALTA INSTANCIAS DE RECURSOS

///////////////////// NO APLICADOS TODAVIA //////////////////////

/*
void kernel_config_set_socket_memoria(t_kernel_config* this, int socketMemoria) 
{
    this->SOCKET_MEMORIA = socketMemoria;
}

int kernel_config_get_socket_dispatch_cpu(t_kernel_config* this) 
{
    return this->SOCKET_DISPATCH_CPU;
}

int kernel_config_get_socket_interrupt_cpu(t_kernel_config* this) 
{
    return this->SOCKET_INTERRUPT_CPU;
}

int kernel_config_get_socket_memoria(t_kernel_config* this) 
{
    return this->SOCKET_MEMORIA;
}
*/