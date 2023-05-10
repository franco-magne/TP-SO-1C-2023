#include <../include/kernel-estado.h>

t_estado* estado_create(t_nombre_estado nombre) 
{
    t_estado* this = malloc(sizeof(*this));
    this->listaProcesos = list_create();
    this->nombreEstado = nombre;
    
    this->mutexEstado = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(this->mutexEstado, NULL);
    this->semaforoEstado = malloc(sizeof(*(this->semaforoEstado)));
    sem_init(this->semaforoEstado, 0, 0);

    return this;
}

void estado_encolar_pcb_atomic(t_estado* estadoDest, t_pcb* targetPcb) 
{
  pthread_mutex_lock(estado_get_mutex(estadoDest));
  list_add(estado_get_list(estadoDest), targetPcb);
  pthread_mutex_unlock(estado_get_mutex(estadoDest));
}


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
    pthread_mutex_lock(estado_get_mutex(this));
    t_pcb* pcb = estado_desencolar_primer_pcb(this);
    pthread_mutex_unlock(estado_get_mutex(this));
    
    return pcb;
}

t_list* estado_get_list(t_estado* this) 
{
    return this->listaProcesos;
}

sem_t* estado_get_sem(t_estado* this) 
{
    return this->semaforoEstado;
}

pthread_mutex_t* estado_get_mutex(t_estado* this) 
{
    return this->mutexEstado;
}

