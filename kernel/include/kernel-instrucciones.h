#ifndef KERNEL_INSTRUCCIONES_H
#define KERNEL_INSTRUCCIONES_H


///////////////////////////////// BIBLOTECAS COMMONS /////////////////
#include <commons/log.h>

//////////////// BIBLOTECAS NUESTRAS /////////////
#include "kernel-estado.h"
#include "pcb.h"

void instruccion_exit(t_pcb* pcbAExit, t_estado* estadoExit);

#endif