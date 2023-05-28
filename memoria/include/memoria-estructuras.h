#ifndef MEMORIA-ESTRUCTURAS_H
#define MEMORIA-ESTRUCTURAS_H

#include <errno.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <sys/socket.h>
#include <../../utils/src/utils/conexiones.h>
#include <commons/log.h>
#include <commons/config.h>
#include <pthread.h>

#include <stdio.h>

#define MEMORIA_CONFIG_UBICACION "config/memoria.config"
#define MEMORIA_LOG_UBICACION "logs/memoria.log"
#define MEMORIA_PROCESS_NAME "Memoria"


typedef struct {
    void* espacio_usuario; // Espacio de usuario
    int tamanio;
} Memoria;

typedef struct {
    int segmento_id;
    int tamanio;
    int base;
} Segmento;

typedef struct {
    Segmento* segmentos;
} TablaSegmentos;

typedef struct {
    int base;
    int tamanio;
} EspacioLibre;

typedef struct {
    EspacioLibre* espacios_libres;
} ListaEspacioLibre;

typedef struct  {

    char* IP_ESCUCHA;   //seria IP_MEMORIA
    char* PUERTO_ESCUCHA;   //seria PUERTO_MEMORIA
    uint32_t TAM_MEMORIA;
    uint32_t TAM_SEGMENTO;
    uint32_t CANT_SEGMENTOS;
    uint32_t RETARDO_MEMORIA;
    uint32_t RETARDO_COMPACTACION;
    char* ALGORITMO_ASIGNACION;

} t_memoria_config;

typedef struct {
    Memoria* memoria;
    TablaSegmentos* tablaSegmentos;
    ListaEspacioLibre* listaEspacioLibre;
    int retardo_acceso_memoria;
    int retardo_compactacion;
    t_log* memoriaLogger;
    int tamMaxDefxCPU;
    //t_memoria_config* memoriaConfig;

} t_MemoriaData;

#endif