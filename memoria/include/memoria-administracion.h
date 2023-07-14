#ifndef MEMORIA_ADMINISTRACION_H
#define MEMORIA_ADMINISTRACION_H

#include <commons/collections/list.h>
#include <../../utils/src/utils/commons-nuestras.h>


#include "memoria-estructuras.h"


bool sobra_espacio_del_hueco_libre(Segmento* nuevoSegmento, Segmento* segmentoReal);
void administrar_primer_hueco_libre(t_list* huecosLibres, Segmento* nuevoSegmento);

Segmento* consolidar_segmentos(Segmento* unSegmento, Segmento* segSiguiente);

uint8_t administrar_nuevo_segmento(Segmento* nuevoSegmento);
void eliminar_segmento_memoria(Segmento* segmentoAEliminar);
void mostrar_tabla(t_list* tablaDeSegmentosSolic);
void mostrar_lista_segmentos(t_list* lista);
bool segmentos_validez_0(Segmento* unSegmento);
bool segmentos_validez_1(Segmento* unSegmento);
void borrar_tabla_de_segmentos_x_pid(uint32_t pid);
void borrar_tabla_de_segmentos_x_pid(uint32_t pid);
void iniciar_compactacion();
void mostrar_lista_segmentos_actualizados_por_compactacion();

#endif