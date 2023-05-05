#include <../include/kernel-estado.h>


void estado_encolar_pcb_atomic(t_estado* estadoDest, t_pcb* targetPcb) 
{
  //  pthread_mutex_lock(estado_get_mutex(estadoDest));
    list_add(estado_get_list(estadoDest), targetPcb);
  //  pthread_mutex_unlock(estado_get_mutex(estadoDest));
}


t_list* estado_get_list(t_estado* this) 
{
    return this->listaProcesos;
}


