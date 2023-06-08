#ifndef KERNEL_MEMORIA_ADAPTER_H
#define KERNEL_MEMORIA_ADAPTER_H

////////////////////////////////// BIBLOTECA STANDAR ///////////////////
#include <string.h>


///////////////////////////////// BIBLOTECAS COMMONS /////////////////
#include <commons/log.h>

//////////////// BIBLOTECAS NUESTRAS /////////////////////////////////
#include "pcb.h"
#include "kernel-config.h"

void mem_adapter_crear_segmento(t_pcb* pcbAIniciar, t_kernel_config* kernelConfig, t_log* kernelLogger);


#endif