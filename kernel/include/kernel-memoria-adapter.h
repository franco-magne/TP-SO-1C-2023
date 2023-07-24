#ifndef KERNEL_MEMORIA_ADAPTER_H
#define KERNEL_MEMORIA_ADAPTER_H

////////////////////////////////// BIBLOTECA STANDAR ///////////////////
#include <string.h>
#include <pthread.h>


///////////////////////////////// BIBLOTECAS COMMONS /////////////////
#include <commons/log.h>
#include "kernel.h"

//////////////// BIBLOTECAS NUESTRAS /////////////////////////////////
#include "pcb.h"
#include "kernel-config.h"
#include "kernel.h"

#include "../../utils/src/utils/conexiones.h"

void memoria_adapter_enviar_create_segment(t_pcb* pcbAIniciar, t_kernel_config* kernelConfig);
uint8_t memoria_adapter_recibir_create_segment(t_pcb* pcbAActualizar, t_kernel_config* kernelConfig, t_log* kernelLogger);
void memoria_adapter_enviar_delete_segment(t_pcb* pcbAIniciar, t_kernel_config* kernelConfig);
void memoria_adapter_recibir_delete_segment(t_pcb* pcbAActualizar, t_kernel_config* kernelConfig, t_log* kernelLogger);
void memoria_adapter_enviar_finalizar_proceso(t_pcb* pcb,  t_kernel_config* kernelConfig, t_log* kernelLogger, char* motivoFinalizacion);





#endif