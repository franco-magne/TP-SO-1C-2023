
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
recurso* recursos_inicializar();



////// FIRMAS DE GETTERS ////////////
uint32_t cpu_pcb_get_program_counter(t_cpu_pcb* self);
uint32_t cpu_pcb_get_pid(t_cpu_pcb* self);
t_list* cpu_pcb_get_instrucciones(t_cpu_pcb* self);
t_registros_cpu* cpu_pcb_get_registros(t_cpu_pcb* self);
char* cpu_pcb_get_registro_ax(t_cpu_pcb* self);
char* cpu_pcb_get_registro_bx(t_cpu_pcb* self);
char* cpu_pcb_get_registro_cx(t_cpu_pcb* self);
char* cpu_pcb_get_registro_dx(t_cpu_pcb* self);
char* cpu_pcb_get_registro_ebx(t_cpu_pcb* self);
char* cpu_pcb_get_registro_eax(t_cpu_pcb* self);
char* cpu_pcb_get_registro_ecx(t_cpu_pcb* self);
char* cpu_pcb_get_registro_edx(t_cpu_pcb* self);
char* cpu_pcb_get_registro_rax(t_cpu_pcb* self);
char* cpu_pcb_get_registro_rbx(t_cpu_pcb* self);
char* cpu_pcb_get_registro_rcx(t_cpu_pcb* self);
char* cpu_pcb_get_registro_rdx(t_cpu_pcb* self);
uint32_t cpu_pcb_get_tiempoIO(t_cpu_pcb* self);
char* cpu_pcb_get_recurso_utilizar(t_cpu_pcb* self);
uint32_t cpu_pcb_get_tamanio_de_segmento(t_cpu_pcb* self);
uint32_t cpu_pcb_get_id_de_segmento(t_cpu_pcb* self);
char* cpu_pcb_get_nombre_archivo(t_cpu_pcb* this);
uint32_t cpu_pcb_get_tamanio_archivo(t_cpu_pcb* this);
uint32_t cpu_pcb_get_puntero_archivo(t_cpu_pcb* this);
uint32_t cpu_pcb_get_base_direccion_fisica(t_cpu_pcb* this);
t_list* cpu_pcb_get_tabla_de_segmento(t_cpu_pcb* this);
uint32_t cpu_pcb_get_cantidad_byte(t_cpu_pcb* this);
uint32_t cpu_pcb_get_desplazamiento_segmento(t_cpu_pcb* this);



////// FIRMAS DE SETTERS /////////////
void cpu_pcb_set_tiempoIO(t_cpu_pcb* self, uint32_t tiempoIO);
void cpu_pcb_set_program_counter(t_cpu_pcb* self, uint32_t programCounter);
void cpu_pcb_set_pid(t_cpu_pcb* self, uint32_t pid);
void cpu_pcb_set_instrucciones(t_cpu_pcb* self, t_list* instrucciones);
void cpu_pcb_set_recurso_utilizar(t_cpu_pcb* self, char* recurso);
void cpu_pcb_set_tamanio_de_segmento(t_cpu_pcb* self, uint32_t tamanio);
void cpu_pcb_set_id_de_segmento(t_cpu_pcb* self, uint32_t id);
void cpu_pcb_set_nombre_archivo(t_cpu_pcb* this, char* nombreArchivo);
void cpu_pcb_set_tamanio_archivo(t_cpu_pcb* this, uint32_t tamanioArchivo);
void cpu_pcb_set_puntero_archivo(t_cpu_pcb* this, uint32_t punteroArchivo);
void cpu_pcb_set_base_direccion_fisica(t_cpu_pcb* this, uint32_t direccionFisica);
void cpu_pcb_set_tabla_de_segmento(t_cpu_pcb* this, t_list* listaDeSegmento);
void cpu_pcb_set_cantidad_byte(t_cpu_pcb* this, uint32_t cantidadByte);
void cpu_pcb_set_desplazamiento_segmento(pcb,desplazamiento_segmento);
void cpu_pcb_set_desplazamiento_segmento(t_cpu_pcb* this, uint32_t desplazamiento);


void cpu_pcb_set_registros(t_cpu_pcb* self, t_registros_cpu* registrosCpu);
void cpu_pcb_set_registro_ax(t_cpu_pcb* self, char* registro);
void cpu_pcb_set_registro_bx(t_cpu_pcb* self, char* registro);
void cpu_pcb_set_registro_cx(t_cpu_pcb* self, char* registro);
void cpu_pcb_set_registro_dx(t_cpu_pcb* self, char* registro);
void cpu_pcb_set_registro_eax(t_cpu_pcb* self, char* registro);
void cpu_pcb_set_registro_ebx(t_cpu_pcb* self, char* registro);
void cpu_pcb_set_registro_ecx(t_cpu_pcb* self, char* registro);
void cpu_pcb_set_registro_edx(t_cpu_pcb* self, char* registro);
void cpu_pcb_set_registro_rax(t_cpu_pcb* self, char* registro);
void cpu_pcb_set_registro_rbx(t_cpu_pcb* self, char* registro);
void cpu_pcb_set_registro_rcx(t_cpu_pcb* self, char* registro);
void cpu_pcb_set_registro_rdx(t_cpu_pcb* self, char* registro);



#endif