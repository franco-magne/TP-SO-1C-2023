#ifndef Kernel-estado_H
#define Kernel-estado_H

/////////////////// BIBLOTECAS BASICAS ////////////////////
#include <stdlib.h>
#include <semaphore.h>  
#include <pthread.h>
/////////////////// BIBLOTECAS COMMONS ////////////////////
#include <commons/log.h>
#include <commons/collections/list.h>
/////////////////// BIBLOTECAS NUESTRAS ///////////////////
#include "pcb.h"


/////////////////// ESTRUCTURAS //////////////////////


typedef struct {

    t_nombre_estado nombreEstado;
    t_list* listaProcesos;
    sem_t* semaforoEstado;
    pthread_mutex_t* mutexEstado;

}t_estado ;

t_estado* estado_create(t_nombre_estado nombre) ;
t_list* estado_get_list(t_estado* this);
void estado_encolar_pcb_atomic(t_estado* estadoDest, t_pcb * targetPcb);
t_pcb* estado_desencolar_primer_pcb(t_estado* this);
t_pcb* estado_desencolar_primer_pcb_atomic(t_estado* this);
sem_t* estado_get_sem(t_estado* this);
t_pcb* estado_remover_pcb_de_cola(t_estado* self, t_pcb* targetPcb);
t_pcb* estado_remover_pcb_de_cola_atomic(t_estado* self, t_pcb* targetPcb);
pthread_mutex_t* estado_get_mutex(t_estado* this);




#endif