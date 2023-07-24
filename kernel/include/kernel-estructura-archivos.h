#ifndef KERNEL_ESTRUCTURA_ARCHIVOS_H
#define KERNEL_ESTRUCTURA_ARCHIVOS_H

////////////////////////////////// BIBLOTECA STANDAR ///////////////////
#include <string.h>

///////////////////////////////// BIBLOTECAS COMMONS /////////////////
#include <commons/log.h>
#include <commons/collections/queue.h>
//////////////// BIBLOTECAS NUESTRAS /////////////////////////////////
//#include "pcb.h"
#include "kernel-config.h"
/////////////////////////////// PARA KERNEL //////////////////////////
typedef struct{

    char* nombreDeArchivo;
    uint32_t pidProceso;
    t_queue* colaDeProcesosEsperandoPorElArchivo;

}t_kernel_archivo;


////////////////////////////// PARA EL PROCESO ////////////////////////
typedef struct{
    char* nombreDeArchivo;
    uint32_t tamanioArchivo;
    uint32_t punteroArchivo;
    uint32_t direccionFisicaArchivo;
    uint32_t cantidadByte;
    uint32_t desplazamientoFisico;
    bool victimaDelArchivo;

}t_pcb_archivo;


//////////////////////// DEFINICIONES FUNCIONES ////////////////////////

t_queue* kernel_archivo_get_cola_procesos_bloqueados(t_kernel_archivo* this);
//void kernel_archivo_aniadir_cola_procesos_bloqueados(t_kernel_archivo* this, t_pcb* unPcb);
t_kernel_archivo* archivo_create_kernel(uint32_t pid, char* nombreArchivo);
t_pcb_archivo* archivo_create_pcb(char* nombreArchivo);
void archivo_pcb_set_victima(t_pcb_archivo* this, bool victimaArchivo);
bool archivo_pcb_get_victima(t_pcb_archivo* this);
char* archivo_motivo_de_bloqueo(t_list* listaDeArchivosDePcb);
bool es_el_archivo_victima(t_pcb_archivo * unArchivo);
char* archivo_pcb_get_nombre_archivo(t_pcb_archivo* this);
bool el_archivo_ya_existe(t_kernel_archivo* unArchivo, t_kernel_archivo* otroArchivo);
void modificar_victima_archivo(t_list* listaDeArchivosDePcb,char* nombreArchivo, bool cambio);
void eliminar_archivo_pcb(t_list* listaDeArchivosDePcb,char* nombreArchivo);
int archivo_kernel_index(t_list* listaDeArchivosDePcb,char* nombreArchivo);
void kernel_archivo_set_cola_procesos_bloqueados(t_kernel_archivo* this , t_queue* cola);
uint32_t archivo_pcb_get_tamanio_archivo(t_pcb_archivo* this);
void archivo_pcb_set_tamanio_archivo(t_pcb_archivo* this, uint32_t tamanio);
int index_de_archivo_pcb(t_list* listaDeArchivosDePcb, char* nombreArchivo);
uint32_t archivo_pcb_get_puntero_archivo(t_pcb_archivo* this);
void archivo_pcb_set_puntero_archivo(t_pcb_archivo* this, uint32_t punteroArchivo);
void archivo_pcb_set_direccion_fisica(t_pcb_archivo* this, uint32_t direccionFisicaArchivo);
uint32_t archivo_pcb_get_direccion_fisica(t_pcb_archivo* this);
uint32_t archivo_pcb_get_cantidad_byte(t_pcb_archivo* this);
uint32_t archivo_pcb_get_desplazamiento_fisico(t_pcb_archivo* this);
void archivo_pcb_set_cantidad_byte(t_pcb_archivo* this, uint32_t cantidadByte);
void archivo_pcb_set_desplazamiento_fisico(t_pcb_archivo* this, uint32_t desplazamiento);


#endif