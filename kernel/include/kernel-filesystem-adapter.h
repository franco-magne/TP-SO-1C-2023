#ifndef KERNEL_FILESYSTEM_ADAPTER_H
#define KERNEL_FILESYSTEM_ADAPTER_H





#include <pthread.h>
//////////////////////////////////////////////// BIBLOTECAS NUESTRAS //////////////////////////////
#include "pcb.h"
#include "kernel-config.h"
#include "kernel-estructura-archivos.h"


bool file_system_adapter_chequear_si_ya_existe(t_pcb* pcb, t_kernel_config* kernelConfig, t_log* kernelLogger);
void file_system_adapter_send_f_open(t_pcb* pcb, t_kernel_config* kernelConfig); 
void file_system_adapter_recv_f_open(t_pcb* pcb, t_kernel_config* kernelConfig);
t_pcb* atender_f_close(char* nombreArchivo);
void file_system_adapter_send_f_truncate(t_pcb* pcb, t_kernel_config* kernelConfig, t_log* kernelLogger);
void file_system_adapter_recv_f_truncate(t_kernel_config* kernelConfig, t_log* kernelLogger);

void instruccion_f_seek(t_pcb* pcbAIniciar, t_kernel_config* kernelConfig, t_log* kernelLogger);

#endif