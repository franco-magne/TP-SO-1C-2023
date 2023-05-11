#ifndef KERNEL-CONFIG_H
#define KERNEL-CONFIG_H

//////////////// BIBLOTECAS STANDARS ///////////////
#include <string.h>
//////////////// BIBLOTECAS COMMONS ////////////////

#include <commons/config.h>
#include <commons/log.h>




//////////////// BIBLOTECAS NUESTRAS ///////////////
#include "utils/commons-nuestras.h"





typedef struct{
    
    char* IP_MEMORIA;
    char* PUERTO_MEMORIA;
    char* IP_FILESYSTEM;
    char* PUERTO_FILESYSTEM;
    char* IP_CPU;
    char* PUERTO_CPU;
    char* IP_ESCUCHA;
    char* PUERTO_ESCUCHA;
    char* ALGORITMO_PLANIFICACION;
    int ESTIMACION_INICIAL;
    int HRRN_ALFA;
    //int SOCKET_MEMORIA;
    int SOCKET_DISPATCH_CPU;
    //int SOCKET_INTERRUPT_CPU;
    int GRADO_MULTIPROGRAMACION;
    char** RECURSOS;
    char** INSTANCIAS_RECURSOS;

}t_kernel_config;

//////////////// FIRMA DE FUNCIONES //////////////////////////
t_kernel_config* kernel_config_create(char* kernelConfigPath, t_log* logger);
t_kernel_config* kernel_config_initializer( t_config* tempCfg);

/////////////  FIRMA DE GETTERS //////////////////////////

char* kernel_config_get_ip_memoria(t_kernel_config* this); 
char* kernel_config_get_puerto_memoria(t_kernel_config* this); 
char* kernel_config_get_ip_file_system(t_kernel_config* this);
char* kernel_config_get_puerto_file_system(t_kernel_config* this);
char* kernel_config_get_ip_cpu(t_kernel_config* this);
char* kernel_config_get_puerto_cpu(t_kernel_config* this); 
char* kernel_config_get_ip_escucha(t_kernel_config* this);
char* kernel_config_get_puerto_escucha(t_kernel_config* this); 
int kernel_config_get_grado_multiprogramacion(t_kernel_config* this); 
bool kernel_config_es_algoritmo_fifo(t_kernel_config* this); 
bool kernel_config_es_algoritmo_HRRN(t_kernel_config* this);
char* kernel_config_get_algoritmo_planificacion(t_kernel_config* this); 
int kernel_config_get_socket_dispatch_cpu(t_kernel_config* this); 

#endif