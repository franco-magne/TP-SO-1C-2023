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
    // Otras estructuras auxiliares según sea necesario
} Memoria;

typedef struct {
    int segmento_id;
    int tamanio;
    int base;
} Segmento;

typedef struct {
    Segmento* segmentos;
    int contador;
} TablaSegmentos;

typedef struct {
    int base;
    int tamanio;
    
} EspacioLibre;

typedef struct {
    EspacioLibre* espacios_libres;
    int contador;
} ListaEspacioLibre;

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