#ifndef MEMORIA_ADAPTER_KERNEL_H
#define MEMORIA_ADAPTER_KERNEL_H

#include <stdint.h>
#include <stdbool.h>
#include <stdnoreturn.h>
#include <string.h>
#include <stdio.h>
#include "memoria-estructuras.h"
#include <../../utils/src/utils/commons-nuestras.h>

/*typedef struct 
{
 uint32_t tamanio_de_segmento;
 uint32_t id_de_segmento;
 uint32_t base_del_segmento;
 bool victima;
 uint32_t pid;

}t_segmento;*/


t_segmento* segmento_kernel_create();
bool segmentos_del_mismo_pid(Segmento* unSegmento, Segmento* otroSegmento);
t_segmento* adapter_segmento_memoria_kernel(Segmento* segmentoAAdaptar);
void buffer_pack_segmento_list(t_buffer* buffer, t_list* lista_segmentos);


#endif

