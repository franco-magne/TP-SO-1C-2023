#include <../include/kernel-algoritmos.h>

////////////////////////////////// FIFO /////////////////////////////

t_pcb* elegir_pcb_segun_fifo(t_estado* estado)
{
    return estado_desencolar_primer_pcb_atomic(estado);
}


//////////////////////////////// HHRN ////////////////////////////////
static double responsable_ratio(double waiting_time , double burst_time) {
    return (waiting_time + burst_time)/burst_time ;
//              W               S       S
// (W+S)/S
}

static double media_exponencial(double estimacionAnterior, double realAnterior) {
    t_config* alfaConfig = config_create("config/kernel.config");
    double alfa = config_get_int_value(alfaConfig, "HRRN_ALFA");

    return alfa * estimacionAnterior + (1 - alfa) * realAnterior;
}

double calcular_siguiente_estimacion(t_pcb* pcb) {
    struct timespec end;
    set_timespec(&end);
    return responsable_ratio(
            obtener_diferencial_de_tiempo_en_milisegundos(end, pcb_get_tiempo_en_ready(pcb)),
            media_exponencial( pcb_get_rafaga_actual(pcb), pcb_get_rafaga_anterior(pcb) )
    );

}

static t_pcb* mayor_ratio(t_pcb* unPcb, t_pcb* otroPcb) {
    double unaRafagaRestante = calcular_siguiente_estimacion(unPcb);
    double otraRafagaRestante = calcular_siguiente_estimacion(otroPcb);

    return unaRafagaRestante >= otraRafagaRestante
               ? unPcb
               : otroPcb;
}


t_pcb* elegir_pcb_segun_hhrn(t_estado* estado) {
    t_pcb* pcbElecto = NULL;
    pthread_mutex_lock(estado_get_mutex(estado));
    int cantidadPcbsEnLista = list_size(estado_get_list(estado));
    if (cantidadPcbsEnLista == 1) {
        pcbElecto = estado_desencolar_primer_pcb(estado);
        pcb_set_estado_anterior(pcbElecto,pcb_get_rafaga_actual(pcbElecto));
    } else if (cantidadPcbsEnLista > 1) {
        pcbElecto = list_get_minimum(estado_get_list(estado), (void*)mayor_ratio);
        estado_remover_pcb_de_cola(estado, pcbElecto);
        pcb_set_estado_anterior(pcbElecto,pcb_get_rafaga_actual(pcbElecto));
    }
    pthread_mutex_unlock(estado_get_mutex(estado));
    return pcbElecto;
}
