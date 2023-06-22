#ifndef MEMORIA_ADMINISTRACION_H
#define MEMORIA_ADMINISTRACION_H

#include <commons/collections/list.h>
#include <../../utils/src/utils/commons-nuestras.h>


#include "memoria-estructuras.h"


bool sobra_espacio_del_hueco_libre(Segmento* nuevoSegmento, Segmento* segmentoReal);
void administrar_primer_hueco_libre(t_list* huecosLibres, Segmento* nuevoSegmento);

Segmento* consolidar_segmentos(Segmento* unSegmento, Segmento* segSiguiente);

void administrar_nuevo_segmento(Segmento* nuevoSegmento);
void eliminar_segmento_memoria(Segmento* segmentoAEliminar);
void mostrar_tabla(t_list* tablaDeSegmentosSolic);


#endif