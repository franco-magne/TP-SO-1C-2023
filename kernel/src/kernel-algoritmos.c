#include <../include/kernel-algoritmos.h>


static double obtener_diferencial_de_tiempo_en_milisegundos(struct timespec end, struct timespec start) 
{
    const uint32_t SECS_TO_MILISECS = 1000;
    const uint32_t NANOSECS_TO_MILISECS = 1000000;
    return (end.tv_sec - start.tv_sec) * SECS_TO_MILISECS + (end.tv_nsec - start.tv_nsec) / NANOSECS_TO_MILISECS;
}

////////////////////////////////// FIFO /////////////////////////////

static t_pcb* elegir_pcb_segun_fifo(t_estado* estado)
{
    return estado_desencolar_primer_pcb_atomic(estado);
}


//////////////////////////////// HHRN ////////////////////////////////
static double responsable_ratio(double waiting_time , double burst_time) {
    return (waiting_time + burst_time)/burst_time ;

}

double calcular_siguiente_estimacion(t_pcb* pcb) {
    struct timespec end;
    clock_gettime(CLOCK_REALTIME, &end);
    //set_timespec(&end);
    return responsable_ratio(
            obtener_diferencial_de_tiempo_en_milisegundos(end, pcb_get_tiempo_en_ready(pcb)),
            pcb_get_instrucciones_buffer(pcb)->size
    );
}

static t_pcb* mayor_ratio(t_pcb* unPcb, t_pcb* otroPcb) {
    double unaRafagaRestante = calcular_siguiente_estimacion(unPcb);
    double otraRafagaRestante = calcular_siguiente_estimacion(otroPcb);
    return unaRafagaRestante <= otraRafagaRestante
               ? unPcb
               : otroPcb;
}


static t_pcb* elegir_pcb_segun_hhrn(t_estado* estado) {
    t_pcb* pcbElecto = NULL;
    pthread_mutex_lock(estado_get_mutex(estado));
    int cantidadPcbsEnLista = list_size(estado_get_list(estado));
    if (cantidadPcbsEnLista == 1) {
        pcbElecto = estado_desencolar_primer_pcb(estado);
    } else if (cantidadPcbsEnLista > 1) {
        pcbElecto = list_get_minimum(estado_get_list(estado), (void*)mayor_ratio);
        estado_remover_pcb_de_cola(estado, pcbElecto);
    }
    pthread_mutex_unlock(estado_get_mutex(estado));
    return pcbElecto;
}
