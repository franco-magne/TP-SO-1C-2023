#ifndef KERNEL_MEMORIA_ADAPTER_H
#define KERNEL_MEMORIA_ADAPTER_H

////////////////////////////////// BIBLOTECA STANDAR ///////////////////
#include <string.h>


///////////////////////////////// BIBLOTECAS COMMONS /////////////////
#include <commons/log.h>

//////////////// BIBLOTECAS NUESTRAS /////////////////////////////////
#include "pcb.h"
#include "kernel-config.h"

#include "../../utils/src/utils/conexiones.h"

void instruccion_create_segment(t_pcb* pcbAIniciar, t_kernel_config* kernelConfig, t_log* kernelLogger);
void instruccion_delete_segment(t_pcb* pcbAIniciar, t_kernel_config* kernelConfig, t_log* kernelLogger);


#endif