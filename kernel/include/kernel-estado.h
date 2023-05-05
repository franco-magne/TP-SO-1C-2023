#ifndef Kernel-estado_H
#define Kernel-estado_H

/////////////////// BIBLOTECAS BASICAS ////////////////////
#include <stdlib.h>
#include <semaphore.h>  
/////////////////// BIBLOTECAS COMMONS ////////////////////
#include <commons/log.h>
#include <commons/collections/list.h>
/////////////////// BIBLOTECAS NUESTRAS ///////////////////
#include "pcb.h"


/////////////////// ESTRUCTURAS //////////////////////


typedef struct {

    t_nombre_estado nombreEstado;
    t_list* listaProcesos;
  //  sem_t* semaforoEstado;

}t_estado;

t_list* estado_get_list(t_estado* this);
void estado_encolar_pcb_atomic(t_estado* estadoDest, t_pcb * targetPcb);

#endif