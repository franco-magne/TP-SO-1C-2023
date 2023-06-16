#ifndef KERNEL_INSTRUCCIONES_H
#define KERNEL_INSTRUCCIONES_H


///////////////////////////////// BIBLOTECAS COMMONS /////////////////
#include <commons/log.h>

//////////////// BIBLOTECAS NUESTRAS /////////////
#include "kernel-estado.h"
#include "pcb.h"
#include "kernel-config.h"
void instruccion_exit(t_pcb* pcbAExit, t_estado* estadoExit);
void instruccion_io(t_pcb* pcb, t_estado* estadoBlocked, t_estado* estadoReady);
void instruccion_yield(t_pcb* pcb,t_estado* estadoReady);
void instruccion_signal(t_pcb* pcb, t_estado* estadoBlocked, t_estado* estadoReady, t_estado* estadoExit);
bool instruccion_wait(t_pcb* pcb, t_estado* estadoBlocked, t_estado* estadoExit );
char* string_pids_ready(t_estado* estadoReady);

#endif