#ifndef KERNEL_FILESYSTEM_ADAPTER_H
#define KERNEL_FILESYSTEM_ADAPTER_H






//////////////////////////////////////////////// BIBLOTECAS NUESTRAS //////////////////////////////
#include "pcb.h"
#include "kernel-config.h"


void instruccion_f_open(t_pcb* pcbAIniciar, t_kernel_config* kernelConfig, t_log* kernelLogger);

void instruccion_f_close(t_pcb* pcbAIniciar, t_kernel_config* kernelConfig, t_log* kernelLogger);

void instruccion_f_truncate(t_pcb* pcbAIniciar, t_kernel_config* kernelConfig, t_log* kernelLogger);

void instruccion_f_seek(t_pcb* pcbAIniciar, t_kernel_config* kernelConfig, t_log* kernelLogger);

#endif