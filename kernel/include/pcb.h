#ifndef PCB_H
#define PCB_H


//////////////////////// BIBLOTECAS BASICAS /////////////////
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <commons/temporal.h>
//////////////////////// BIBLOTECAS COMMONS /////////////////
#include <commons/log.h>
#include <commons/collections/list.h>
//////////////////////// BIBLOTECAS UTILS NUESTRA /////////////////
#include "../../utils/src/utils/serializacion.h"
#include "../../utils/src/utils/instrucciones.h"
#include "../../utils/src/utils/commons-nuestras.h"
#include "kernel-estructura-archivos.h"
/////////////////////// ESTRUCTURA DEL PCB ////////////////////////
typedef struct {
    uint32_t pid;
    uint32_t programCounter;
    uint32_t tiempoIO;
    int socketConsola;  
    t_registros_cpu* registros; 
    t_buffer* instruccionesBuffer;
    t_nombre_estado estadoActual;
    t_nombre_estado estadoAnterior;
    char* recursoUtilizado;
    t_temporal* tiempo_ready;
    double rafaga_anterior;
    double rafaga_actual;
    double estimacionActual;
    uint32_t tamanio_de_segmento;
    uint32_t id_de_segmento;
    t_list *listaDeSegmento;
    t_list *listaArchivosAbiertos;
}t_pcb;

////////////////////////// DEFINICION DE LAS FUNCIONES ////////////////
t_pcb* pcb_create(uint32_t pid);
bool pcb_es_este_pcb_por_pid(void* unPcb, void* otroPcb);
void pcb_destroy(t_pcb* self);

///////////////////////////// SEGMENTO ////////////////////////////////////////////

void segmento_destroy(t_segmento* this);
uint32_t segmento_get_id_de_segmento(t_segmento* this);
uint32_t segmento_get_tamanio_de_segmento(t_segmento* this);
uint32_t segmento_get_base_de_segmento(t_segmento* this);
t_segmento* segmento_victima(t_list* this);
void segmento_set_victima(t_segmento* this, bool cambioEstado);
bool segmento_get_victima(t_segmento* this);
uint32_t index_posicion_del_segmento_victima(t_list* this, uint32_t id, uint32_t pid);
void modificar_victima_lista_segmento(t_list* this, uint32_t id_victima, uint32_t pid, bool cambioVictima);

t_list* buffer_unpack_segmento_list(t_buffer* buffer);
void segmento_set_pid(t_segmento* this, uint32_t pid);
uint32_t segmento_get_pid(t_segmento* this);
bool es_el_segmento_pid(t_segmento* unSegmento, t_segmento* otroSegmento);

/////////////////////// GETTERS ////////////////////////
t_registros_cpu* pcb_get_registros_cpu(t_pcb* this);
uint32_t pcb_get_program_counter(t_pcb* this);
uint32_t pcb_get_pid(t_pcb* this);
int pcb_get_socket_consola(t_pcb* this);
t_buffer* pcb_get_instrucciones_buffer(t_pcb* this);
t_nombre_estado pcb_get_estado_actual(t_pcb* this);
t_nombre_estado pcb_get_estado_anterior(t_pcb* this);
uint32_t pcb_get_tiempoIO(t_pcb* this);
char* pcb_get_recurso_utilizado(t_pcb* this);
t_temporal* pcb_get_tiempo_en_ready(t_pcb* this);
double pcb_get_estimacion_anterior(t_pcb* this);
double pcb_get_rafaga_anterior(t_pcb* this);
t_list* pcb_get_lista_de_segmentos(t_pcb* this);
t_list* pcb_get_lista_de_archivos_abiertos(t_pcb* this);
/////////////////////// SETTERS ////////////////////////
void pcb_set_program_counter(t_pcb* this, uint32_t programCounter); 
void pcb_set_instructions_buffer(t_pcb* this, t_buffer* instructionsBuffer);
void pcb_set_pid(t_pcb* this, uint32_t pid);
void pcb_set_socket_consola(t_pcb* this, int socket);
void pcb_set_estado_actual(t_pcb* this, uint32_t pid);
void pcb_set_estado_anterior(t_pcb* this, uint32_t estadoAnterior);
void pcb_set_tiempoIO(t_pcb*, uint32_t tiempoIO);
void pcb_set_recurso_utilizado(t_pcb* this, char* recurso);
void pcb_set_tiempo_en_ready(t_pcb* this, struct timespec tiempo_en_ready);
void pcb_set_estimacion_anterior(t_pcb* this,double );
void pcb_set_rafaga_anterior(t_pcb* this,double );
void pcb_set_lista_de_segmentos(t_pcb* this, t_list* unSegmento);
void pcb_add_lista_de_archivos(t_pcb* this,t_pcb_archivo* unArchivo );

void pcb_set_registro_ax_cpu(t_pcb* this, char* registro);
void pcb_set_registro_bx_cpu(t_pcb* this, char* registro);
void pcb_set_registro_cx_cpu(t_pcb* this, char* registro);
void pcb_set_registro_dx_cpu(t_pcb* this, char* registro);
void pcb_set_registro_eax_cpu(t_pcb* this, char* registro);
void pcb_set_registro_ebx_cpu(t_pcb* this, char* registro);
void pcb_set_registro_ecx_cpu(t_pcb* this, char* registro);
void pcb_set_registro_edx_cpu(t_pcb* this, char* registro);
void pcb_set_registro_rax_cpu(t_pcb* this, char* registro);
void pcb_set_registro_rbx_cpu(t_pcb* this, char* registro);
void pcb_set_registro_rcx_cpu(t_pcb* this, char* registro);
void pcb_set_registro_rdx_cpu(t_pcb* this, char* registro);


#endif