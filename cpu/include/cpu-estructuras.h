#ifndef CPU_ESTRUCTURAS_H
#define CPU_ESTRUCTURAS_H


//Standard libraries
#include <stdlib.h>
#include <stdint.h>

//Commons libraries
#include <commons/log.h>
#include <commons/config.h>
//Bibliotecas static-utils
#include "../../utils/src/utils/instrucciones.h"
//Bibliotecas internas modulo consola

#define CPU_MODULE_NAME "Cpu"

typedef struct  {

    uint32_t ENTRADAS_TLB;
    char* REEMPLAZO_TLB;
    uint32_t RETARDO_INSTRUCCION;
    char* IP_MEMORIA;
    char* PUERTO_MEMORIA;
    char* IP_ESCUCHA;
    char* PUERTO_ESCUCHA_DISPATCH;
    char* PUERTO_ESCUCHA_INTERRUPT;
    uint32_t TAMANIO_PAGINA;
    uint32_t ENTRADAS_POR_TABLA;
    int SOCKET_MEMORIA;
    int SOCKET_DISPATCH_CPU;
    int SOCKET_INTERRUPT_CPU;

} t_cpu_config;


// Aca coincidimos los datos que manda el buffer de kernel-adapter
typedef struct  {
    uint32_t pid;
    uint32_t programCounter;
    uint32_t* arrayTablaPaginas; // TP "de ellos" no usan un puntero 
    uint32_t* arrayDeSegmentos;
    t_list* instrucciones;
    t_registros_cpu* registrosCpu;
} t_cpu_pcb;

extern t_log* cpuMinimalLogger;
extern t_log* cpuLogger;
extern t_cpu_config* cpuConfig;


void cpu_config_initializer(void* moduleConfig, t_config* tempCfg);
void cpu_config_destroy(t_cpu_config* self);
t_cpu_config* cpu_config_create(char* cpuConfigPath, t_log* cpuLogger) ;
uint32_t cpu_config_get_retardo_instruccion(t_cpu_config* self);
char* cpu_config_get_ip_memoria(t_cpu_config* self);
char* cpu_config_get_puerto_memoria(t_cpu_config* self);
char* cpu_config_get_puerto_dispatch(t_cpu_config* self); 
char* cpu_config_get_ip_cpu(t_cpu_config* self);
int cpu_config_get_socket_dispatch(t_cpu_config* self); 
int cpu_config_get_socket_memoria(t_cpu_config* self);
void cpu_config_set_socket_memoria(t_cpu_config* self, int socketMemoria);
void cpu_config_set_socket_dispatch(t_cpu_config* self, int socketDispatch); 

#endif