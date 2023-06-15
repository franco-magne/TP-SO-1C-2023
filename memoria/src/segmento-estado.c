#include <../include/segmento-estado.h>
/*
t_estado* estado_create() 
{
    t_estado* this = malloc(sizeof(*this));
    this->listaProcesos = list_create();
    
    this->mutexEstado = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(this->mutexEstado, NULL);
    

    return this;
}

void estado_encolar_segmento_atomic(t_estado* estadoDest, Segmento* targetSegmento) 
{
  pthread_mutex_lock(estado_get_mutex(estadoDest));
  list_add(estado_get_list(estadoDest), targetSegmento);
  pthread_mutex_unlock(estado_get_mutex(estadoDest));
}


Segmento* estado_desencolar_primer_segmento(t_estado* this){

    if(list_is_empty(this)){
      exit(EXIT_FAILURE);
    }
    else{
      return list_remove(estado_get_list(this), 0); //es 1 xq el 0 no se borra nunca
    }
}

Segmento* estado_desencolar_primer_segmento_atomic(t_estado* this) 
{
    pthread_mutex_lock(estado_get_mutex(this));
    Segmento* segmento = estado_desencolar_primer_segmento(this);
    pthread_mutex_unlock(estado_get_mutex(this));
    
    return segmento;
}

t_list* estado_get_list(t_estado* this) 
{
    return this->listaProcesos;
}


pthread_mutex_t* estado_get_mutex(t_estado* this) 
{
    return this->mutexEstado;
}

bool segmento_es_este_segmento_por_id_y_socket(void* unSegmento, void* otroSegmento) 
{
    return segmento_get_id((Segmento*)unSegmento) == segmento_get_id((Segmento*)otroSegmento) && segmento_get_socket((Segmento*)otroSegmento) == segmento_get_socket((Segmento*)unSegmento);
}



Segmento* estado_remover_segmento_de_cola(t_estado* self, Segmento* targetSegmento) 
{
    Segmento* segmento = NULL;
    uint32_t index = list_get_index(estado_get_list(self), segmento_es_este_segmento_por_id_y_socket, targetSegmento);
    if (index != -1) {
        segmento = list_remove(estado_get_list(self), index);
    }
    return segmento;
}


Segmento* estado_remover_segmento_de_cola_atomic(t_estado* self, Segmento* targetSegmento) {
    pthread_mutex_lock(estado_get_mutex(self));
    Segmento* segmento = estado_remover_segmento_de_cola(self, targetSegmento);
    pthread_mutex_unlock(estado_get_mutex(self));
    return segmento;
}*/