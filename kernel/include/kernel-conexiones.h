#ifndef KERNEL_CONEXIONES_H
#define KERNEL_CONEXIONES_H

//////////////// BIBLOTECAS STANDARS ///////////////
#include <string.h>
//////////////// BIBLOTECAS COMMONS ////////////////

#include <commons/config.h>
#include <commons/log.h>



//////////////// BIBLOTECAS NUESTRAS ///////////////
#include "../../utils/src/utils/instrucciones.h"
#include "pcb.h"
#include "kernel-estado.h"
#include <utils/conexiones.h>
#include "kernel-config.h"


int conectar_a_servidor_cpu_dispatch( t_kernel_config* kernelConfig, t_log* kernelLogger);
int conectar_con_servidor_file_system(t_kernel_config* kernelConfig, t_log* kernelLogger);
int conectar_con_servidor_memoria(t_kernel_config* kernelConfig, t_log* kernelLogger);

#endif