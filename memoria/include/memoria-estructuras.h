#ifndef MEMORIA_ESTRUCTURAS_H
#define MEMORIA_ESTRUCTURAS_H

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
} t_memoria;

typedef struct {
    //int segmento_id; no necesito idSeg xq me lo indica el indice en que se encuentra en la tabla
    int limite; //tamanio
    int base;
} t_segmento;

typedef struct {
    t_segmento segmentos; 
    int validez;   //0 o 1
} t_entrada_por_tabla;

typedef struct {
    t_entrada_por_tabla* entradas;
} t_tabla_segmentos;

typedef struct {
    int base;
    int tamanio;
} t_espacio_libre;
t_list espaciosVacios;

typedef struct {
    t_espacio_libre* espacios_libres;
} t_lista_espacios_libres;

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


#endif