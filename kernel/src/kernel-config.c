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
    kernelConfig->ESTIMACION_INICIAL = config_get_double_value(tempCfg, "ESTIMACION_INICIAL");
    kernelConfig->HRRN_ALFA = strdup(config_get_string_value(tempCfg, "HRRN_ALFA"));
    kernelConfig->SOCKET_MEMORIA = -1;
    kernelConfig->SOCKET_DISPATCH_CPU = -1;
    kernelConfig->SOCKET_FILE_SYSTEM = -1;
    kernelConfig->GRADO_MULTIPROGRAMACION = config_get_int_value(tempCfg, "GRADO_MAX_MULTIPROGRAMACION");
    kernelConfig->RECURSOS = config_get_array_value(tempCfg, "RECURSOS");
    kernelConfig->INSTANCIAS_RECURSOS = config_get_array_value(tempCfg, "INSTANCIAS_RECURSOS");
    
    return kernelConfig;
}


t_kernel_recurso* iniciar_estructuras_de_recursos(int cantidad_de_recursos, char** instancias, char** listarecurso) {
    t_kernel_recurso* recursos = malloc(sizeof(*recursos) * cantidad_de_recursos);
    
    for (int i = 0; i < cantidad_de_recursos; i++) {
        recursos[i].recurso = listarecurso[i];
        recursos[i].instancias_recurso = malloc(sizeof(int));  // Asignar memoria para instancias_recurso
        *(recursos[i].instancias_recurso) = atoi(instancias[i]);  // Almacenar el valor convertido
    }
    
    return recursos;
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

char** kernel_config_get_recurso(t_kernel_config* this){
    return this->RECURSOS;
}
char** kernel_config_get_instancias(t_kernel_config* this){
    return this->INSTANCIAS_RECURSOS;
}

double kernel_config_get_estimacion_inicial(t_kernel_config* this){
    return this->ESTIMACION_INICIAL;
}

void kernel_config_set_socket_dispatch_cpu(t_kernel_config* self, int socketDispatch) 
{
    self->SOCKET_DISPATCH_CPU = socketDispatch;
}
void kernel_config_set_socket_memoria(t_kernel_config* this, int socketMemoria) 
{
    this->SOCKET_MEMORIA = socketMemoria;
}


void kernel_config_set_socket_file_system(t_kernel_config* this, int socketFileSystem)
{
    this->SOCKET_FILE_SYSTEM = socketFileSystem;
}

int kernel_config_get_socket_dispatch_cpu(t_kernel_config* this) 
{
    return this->SOCKET_DISPATCH_CPU;
}

int kernel_config_get_socket_file_system(t_kernel_config* this)
{
    return this->SOCKET_FILE_SYSTEM;
}

int kernel_config_get_socket_memoria(t_kernel_config* this) 
{
    return this->SOCKET_MEMORIA;
}


char* recurso_get_nombre_recurso(t_kernel_recurso* this)
{
    return this->recurso;
}

int* recurso_get_instancias_recurso(t_kernel_recurso* this)
{
    return this->instancias_recurso;
}
