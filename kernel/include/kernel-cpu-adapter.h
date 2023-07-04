#ifndef KERNEL_CPU_ADAPTER_H
#define KERNEL_CPU_ADAPTER_H

/////////////////// BIBLOTECAS BASICAS ////////////////////
#include <stdlib.h>
#include <sys/socket.h>
#include <pthread.h>
/////////////////// BIBLOTECAS COMMONS ////////////////////
#include <commons/log.h>
#include <commons/collections/list.h>
/////////////////// BIBLOTECAS NUESTRAS ///////////////////
#include "pcb.h"
#include "kernel-estado.h"
#include "kernel-config.h"
#include "../../utils/src/utils/instrucciones.h"
#include "utils/serializacion.h"
#include <utils/conexiones.h>
#include "kernel.h"

t_pcb *cpu_adapter_recibir_pcb_actualizado_de_cpu(t_pcb *, uint8_t cpuResponse, t_kernel_config *, t_log *);
void cpu_adapter_enviar_pcb_a_cpu(t_pcb *, uint8_t header, t_kernel_config *, t_log *);



#endif