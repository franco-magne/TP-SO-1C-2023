#ifndef COMMONS_NUESTRAS_H
#define COMMONS_NUESTRAS_H

#include <commons/collections/list.h>
#include <commons/config.h>

#include <commons/log.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include "instrucciones.h"
#include "serializacion.h"
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <readline/readline.h>


// MACROS DE UBICACION DE LAS CONFIG DE LOS PROCESOS

#define KERNEL_CONFIG_UBICACION "config/kernelmemoria.config"
#define FILESYSTEM_CONFIG_UBICACION "config/fileSystem.config"
#define SUPERBLOQUE_CONFIG_UBICACION "config/superbloque.config"
#define MEMORIA_CONFIG_UBICACION "config/memoriabase.config"
#define CPU_CONFIG_UBICACION "config/cpumemoria.config"


typedef struct {
    uint32_t tamanio_de_segmento;
    uint32_t id_de_segmento;
    uint32_t base_del_segmento;
    bool victima;
    uint32_t pid;
} t_segmento;


typedef struct {

    uint32_t baseSegmento;
    uint32_t desplazamiento;
    uint32_t cantidadByte;

} t_direccionFisica;


FILE* abrir_archivo(const char* pathArchivo, const char* mode, t_log* moduloLogger);
int config_init(void* moduleConfig, char* pathToConfig, t_log* moduleLogger, void (*config_initializer)(void* moduleConfig, t_config* tempConfig));
void intervalo_de_pausa(uint32_t duracionEnMilisegundos);
int size_recurso_list(char **);
bool contains(char** list, char* object );
int position_in_list(char** list, char* object );
int list_get_index(t_list* list, bool (*cutting_condition)(void*, void*), void* target);
void set_timespec(struct timespec* timespec);
double obtener_diferencial_de_tiempo_en_milisegundos(struct timespec end, struct timespec start);
bool es_el_ultimo_elemento(t_list* lista, t_link_element* elemento);
t_list* list_filter_ok(t_list* lista, bool (*condition)(void*, void*), void* argumento);
t_list* buffer_unpack_segmento_list(t_buffer* buffer);
t_segmento* buffer_unpack_segmento(t_buffer* buffer);
void buffer_pack_segmento(t_buffer* buffer, t_segmento* segmento);
void buffer_pack_segmento_list(t_buffer* buffer, t_list* lista_segmentos);
t_segmento* segmento_create(uint32_t id_de_segmento, uint32_t tamanio_de_segmento);


void inicio_kernel();
void inicio_cpu();
void imprimir_consola();
void imprimir_memoria();
void imprimir_file_system();


// Colores de texto
#define RESET   "\x1B[0m"
#define RED     "\x1B[31m"
#define GREEN   "\x1B[32m"
#define YELLOW  "\x1B[33m"
#define BLUE    "\x1B[34m"
#define MAGENTA "\x1B[35m"
#define CYAN    "\x1B[36m"
#define WHITE   "\x1B[37m"

// Estilos de texto
#define BOLD    "\x1B[1m"
#define UNDERLINE "\x1B[4m"
#define BLINK "\x1B[5m"
#define STRIKETHROUGH "\x1B[9m"
#define ITALIC "\x1B[3m"

#define BOLDRED "\x1B[1;31m"
#define BOLDGREEN "\x1B[1;32m"
#define BOLDYELLOW "\x1B[1;33m"
#define BOLDBLUE "\x1B[1;34m"
#define BOLDMAGENTA "\x1B[1;35m"
#define BOLDCYAN "\x1B[1;36m"
#define BOLDWHITE "\x1B[1;37m"

#define BACKGROUND_RED "\033[41m"   // Fondo rojo
#define BACKGROUND_GREEN "\033[42m" // Fondo verde
#define BACKGROUND_BLUE "\033[44m"  // Fondo azul

char* getIPAddress();


#endif