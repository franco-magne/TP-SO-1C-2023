#ifndef SEGMENTO_ESTADO_H
#define SEGMENTO_ESTADO_H

#include <errno.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <sys/socket.h>
#include <../../utils/src/utils/conexiones.h>
#include <../../utils/src/utils/serializacion.h>
#include <commons/log.h>
#include <commons/config.h>
#include <pthread.h>

#include <stdio.h>
#include "memoria-config.h"
#include "memoria-atender-kernel.h"
#include "memoria-estructuras.h"

typedef struct {
    t_list* listaProcesos;
    pthread_mutex_t* mutexEstado;
}t_estado;    


t_estado* estado_create() ;
t_list* estado_get_list(t_estado* this);
void estado_encolar_segmento_atomic(t_estado* estadoDest, Segmento* segmento);
Segmento* estado_desencolar_primer_segmento(t_estado* this);
Segmento* estado_desencolar_primer_segmento_atomic(t_estado* this);

Segmento* estado_remover_segmento_de_cola(t_estado* this, Segmento* );
Segmento* estado_remover_segmento_de_cola_atomic(t_estado* this, Segmento* );
pthread_mutex_t* estado_get_mutex(t_estado* this);



#endif