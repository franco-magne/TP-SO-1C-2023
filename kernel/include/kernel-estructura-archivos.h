#ifndef KERNEL_ESTRUCTURA_ARCHIVOS_H
#define KERNEL_ESTRUCTURA_ARCHIVOS_H

////////////////////////////////// BIBLOTECA STANDAR ///////////////////
#include <string.h>

///////////////////////////////// BIBLOTECAS COMMONS /////////////////
#include <commons/log.h>
#include <commons/collections/queue.h>
//////////////// BIBLOTECAS NUESTRAS /////////////////////////////////

/////////////////////////////// PARA KERNEL //////////////////////////
typedef struct{

    char* nombreDeArchivo;
    uint32_t pidProceso;
    t_queue* colaDeProcesosEsperandoPorElArchivo;

}t_kernel_archivo;


////////////////////////////// PARA EL PROCESO ////////////////////////
typedef struct{
    char* nombreDeArchivo;
    bool victimaDelArchivo;

}t_pcb_archivo;



#include "pcb.h"
#include "kernel-config.h"

//////////////////////// DEFINICIONES FUNCIONES ////////////////////////

bool el_archivo_ya_existe(char* unArchivo, char* otroArchivo);
t_queue* kernel_archivo_get_cola_procesos_bloqueados(t_kernel_archivo* this);
void kernel_archivo_aniadir_cola_procesos_bloqueados(t_kernel_archivo* this, t_pcb* unPcb);
t_kernel_archivo* archivo_create_kernel(uint32_t pid, char* nombreArchivo);
t_pcb_archivo* archivo_create_pcb(char* nombreArchivo);
void archivo_pcb_set_victima(t_pcb_archivo* this, bool victimaArchivo);
bool archivo_pcb_get_victima(t_pcb_archivo* this);




#endif