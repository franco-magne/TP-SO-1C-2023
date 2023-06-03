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


t_pcb* estado_remover_pcb_de_cola(t_estado* self, t_pcb* targetPcb) 
{
    t_pcb* pcb = NULL;
    uint32_t index = list_get_index(estado_get_list(self), pcb_es_este_pcb_por_pid, targetPcb);
    if (index != -1) {
        pcb = list_remove(estado_get_list(self), index);
    }
    return pcb;
}


t_pcb* estado_remover_pcb_de_cola_atomic(t_estado* self, t_pcb* targetPcb) {
    pthread_mutex_lock(estado_get_mutex(self));
    t_pcb* pcb = estado_remover_pcb_de_cola(self, targetPcb);
    pthread_mutex_unlock(estado_get_mutex(self));
    return pcb;
}
/*
bool estado_contiene_pcb_atomic(t_estado* self, t_pcb* targetPcb) {
    pthread_mutex_lock(estado_get_mutex(self));
    bool contains = false;
    uint32_t index = list_get_index(estado_get_list(self), pcb_es_este_pcb_por_pid, targetPcb);
    if (index != -1) {
        contains = true;
    }
    pthread_mutex_unlock(estado_get_mutex(self));
    return contains;
}

*/

