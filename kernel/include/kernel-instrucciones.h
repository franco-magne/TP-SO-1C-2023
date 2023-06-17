#ifndef KERNEL_INSTRUCCIONES_H
#define KERNEL_INSTRUCCIONES_H


///////////////////////////////// BIBLOTECAS COMMONS /////////////////
#include <commons/log.h>

//////////////// BIBLOTECAS NUESTRAS /////////////
#include "kernel.h"
#include "kernel-estado.h"
#include "pcb.h"
#include "kernel-config.h"
#include "kernel-memoria-adapter.h"

void instruccion_exit(t_pcb* pcbAExit);
void instruccion_io(t_pcb* pcb);
void instruccion_yield(t_pcb* pcb);
void instruccion_signal(t_pcb* pcb);
bool instruccion_wait(t_pcb* pcb);
void instruccion_create_segment(t_pcb* pcb);
void instruccion_delete_segment(t_pcb* pcb);
bool instruccion_f_open(t_pcb* pcb);
void instruccion_f_close(t_pcb* pcb);

char* string_pids_ready(t_estado* estadoReady);

#endif