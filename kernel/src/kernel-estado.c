#include <../include/kernel-estado.h>



void estado_encolar_pcb_atomic(t_estado* estadoDest, t_pcb* targetPcb) 
{
  //pthread_mutex_lock(estado_get_mutex(estadoDest));
  list_add(estado_get_list(estadoDest), targetPcb);
  //pthread_mutex_unlock(estado_get_mutex(estadoDest));
}


t_list* estado_get_list(t_estado* this)
{   
    return this->listaProcesos;
}
/*
t_pcb* estado_desencolar_primer_pcb(t_estado* this){

    if(list_is_empty(this)){
      exit(EXIT_FAILURE);
    }
    else{
      return list_remove(estado_get_list(this), 0); 
    }
}

t_pcb* estado_desencolar_primer_pcb_atomic(t_estado* this) 
{
   // pthread_mutex_lock(estado_get_mutex(this));
    t_pcb* pcb = estado_desencolar_primer_pcb(this);
   // pthread_mutex_unlock(estado_get_mutex(this));
    
    return pcb;
}
*/
