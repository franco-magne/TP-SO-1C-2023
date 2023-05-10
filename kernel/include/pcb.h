#ifndef PCB_H
#define PCB_H


//////////////////////// BIBLOTECAS BASICAS /////////////////
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
//////////////////////// BIBLOTECAS COMMONS /////////////////
#include <commons/log.h>

//////////////////////// BIBLOTECAS UTILS NUESTRA /////////////////
#include "../../utils/src/utils/serializacion.h"
#include "../../utils/src/utils/instrucciones.h"

/////////////////////// ESTRUCTURA DEL PCB ////////////////////////
typedef struct {
    uint32_t pid;
    uint32_t programCounter;
    t_registro registros;
    t_buffer* instruccionesBuffer;
    t_nombre_estado estadoActual;

}t_pcb;

////////////////////////// DEFINICION DE LAS FUNCIONES ////////////////
t_pcb* pcb_create(uint32_t pid);
uint32_t pcb_get_program_counter(t_pcb* this);
uint32_t pcb_get_pid(t_pcb* this);
t_buffer* pcb_get_instrucciones_buffer(t_pcb* this);
t_nombre_estado pcb_get_estado_actual(t_pcb* this);


/////////////////////// SETTER ////////////////////////

void pcb_set_program_counter(t_pcb* this, uint32_t programCounter); 
void pcb_set_instructions_buffer(t_pcb* this, t_buffer* instructionsBuffer);
void pcb_set_pid(t_pcb* this, uint32_t pid);
void pcb_set_estado_actual(t_pcb* this, uint32_t pid);



#endif