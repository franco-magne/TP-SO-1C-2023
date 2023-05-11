
#ifndef CPU_PCB_H_INCLUDED
#define CPU_PCB_H_INCLUDED

//Standard Libraries
#include <stdint.h>
//Static-utils libraries
#include "../../utils/src/utils/instrucciones.h"
//Commons libraries
#include <commons/collections/list.h>
//Internal cpu libraries
#include "cpu-estructuras.h"


t_cpu_pcb* cpu_pcb_create(uint32_t pid, uint32_t programCounter, t_registros_cpu* registrosCpu);
void cpu_pcb_destroy(t_cpu_pcb* self);

////// FIRMAS DE GETTERS
uint32_t cpu_pcb_get_program_counter(t_cpu_pcb* self);
uint32_t cpu_pcb_get_pid(t_cpu_pcb* self);
//uint32_t* cpu_pcb_get_array_tabla_paginas(t_cpu_pcb* self);
t_list* cpu_pcb_get_instrucciones(t_cpu_pcb* self);
//uint32_t* cpu_pcb_get_array_segmentos(t_cpu_pcb* self);
t_registros_cpu* cpu_pcb_get_registros(t_cpu_pcb* self);
uint32_t cpu_pcb_get_registro_ax(t_cpu_pcb* self);
uint32_t cpu_pcb_get_registro_bx(t_cpu_pcb* self);
uint32_t cpu_pcb_get_registro_cx(t_cpu_pcb* self);
uint32_t cpu_pcb_get_registro_dx(t_cpu_pcb* self);
uint32_t cpu_pcb_get_registro_eax(t_cpu_pcb* self);
uint32_t cpu_pcb_get_registro_ebx(t_cpu_pcb* self);
uint32_t cpu_pcb_get_registro_ecx(t_cpu_pcb* self);
uint32_t cpu_pcb_get_registro_edx(t_cpu_pcb* self);
uint32_t cpu_pcb_get_registro_rax(t_cpu_pcb* self);
uint32_t cpu_pcb_get_registro_rbx(t_cpu_pcb* self);
uint32_t cpu_pcb_get_registro_rcx(t_cpu_pcb* self);
uint32_t cpu_pcb_get_registro_rdx(t_cpu_pcb* self);

////// FIRMAS DE SETTERS 
void cpu_pcb_set_program_counter(t_cpu_pcb* self, uint32_t programCounter);
void cpu_pcb_set_pid(t_cpu_pcb* self, uint32_t pid);
//void cpu_pcb_set_array_tabla_paginas(t_cpu_pcb* self, uint32_t* tablaPaginas);
void cpu_pcb_set_instrucciones(t_cpu_pcb* self, t_list* instrucciones);
//void cpu_pcb_set_array_segmentos(t_cpu_pcb* self, uint32_t* arrayDeSegmentos);
void cpu_pcb_set_registros(t_cpu_pcb* self, t_registros_cpu* registrosCpu);
void cpu_pcb_set_registro_ax(t_cpu_pcb* self, uint32_t registro);
void cpu_pcb_set_registro_bx(t_cpu_pcb* self, uint32_t registro);
void cpu_pcb_set_registro_cx(t_cpu_pcb* self, uint32_t registro);
void cpu_pcb_set_registro_dx(t_cpu_pcb* self, uint32_t registro);
void cpu_pcb_set_registro_eax(t_cpu_pcb* self, uint32_t registro);
void cpu_pcb_set_registro_ebx(t_cpu_pcb* self, uint32_t registro);
void cpu_pcb_set_registro_ecx(t_cpu_pcb* self, uint32_t registro);
void cpu_pcb_set_registro_edx(t_cpu_pcb* self, uint32_t registro);
void cpu_pcb_set_registro_rax(t_cpu_pcb* self, uint32_t registro);
void cpu_pcb_set_registro_rbx(t_cpu_pcb* self, uint32_t registro);
void cpu_pcb_set_registro_rcx(t_cpu_pcb* self, uint32_t registro);
void cpu_pcb_set_registro_rdx(t_cpu_pcb* self, uint32_t registro);

#endif