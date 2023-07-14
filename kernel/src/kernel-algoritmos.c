#include <../include/kernel-algoritmos.h>

extern t_kernel_config* kernelConfig;
extern t_log* kernelLogger;

////////////////////////////////// FIFO /////////////////////////////

t_pcb* elegir_pcb_segun_fifo(t_estado* estado)
{
    return estado_desencolar_primer_pcb_atomic(estado);
}


//////////////////////////////// HRRN ////////////////////////////////
static double calcularHRRN(double tiempoDeEspera , double rafagaEstimadaProxima) {
    return (1 + (tiempoDeEspera / rafagaEstimadaProxima));

}

static double calcularEstimacionProxima(double estimacionAnterior, double realAnterior) {
    double alfa = strtod(kernelConfig->HRRN_ALFA ,NULL);

    return alfa * realAnterior  + (1 - alfa) * estimacionAnterior;
}

double calcular_siguiente_estimacion(t_pcb* pcb) {
  
//struct timespec tiempoFinalSpec;
//struct timespec tiempoEnReadySpec;
double tiempoDeEspera = 0;
double resultado = 0;
double resultadoEstimacionProxima = 0;

if(pcb_get_estimacion_anterior(pcb) == 0){
resultado = calcularHRRN((double)temporal_gettime(pcb->tiempo_ready), kernel_config_get_estimacion_inicial(kernelConfig));
} else {

resultadoEstimacionProxima = calcularEstimacionProxima( pcb_get_estimacion_anterior(pcb) , pcb_get_rafaga_anterior(pcb) );
resultado = calcularHRRN((double)temporal_gettime(pcb->tiempo_ready), resultadoEstimacionProxima);
}

// 

//set_timespec(&tiempoFinalSpec);

//tiempoEnReadySpec = pcb_get_tiempo_en_ready(pcb);

//tiempoDeEspera = obtener_diferencial_de_tiempo_en_milisegundos(tiempoFinalSpec,tiempoEnReadySpec);

//temporal_stop(pcb->tiempo_ready);



//temporal_resume(pcb->tiempo_ready);
return resultado;

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
    double setearEstimacionAnteriorPCBelecto;
    pthread_mutex_lock(estado_get_mutex(estado));
    int cantidadPcbsEnLista = list_size(estado_get_list(estado));
    if (cantidadPcbsEnLista == 1) {
        pcbElecto = estado_desencolar_primer_pcb(estado);
    } else if (cantidadPcbsEnLista > 1) {
        pcbElecto = list_get_minimum(estado_get_list(estado), (void*)mayor_ratio);
        estado_remover_pcb_de_cola(estado, pcbElecto);

    }
    pthread_mutex_unlock(estado_get_mutex(estado));

    
    if( pcb_get_estimacion_anterior(pcbElecto) !=0 ){
    setearEstimacionAnteriorPCBelecto =  calcularEstimacionProxima( pcb_get_estimacion_anterior(pcbElecto) , pcb_get_rafaga_anterior(pcbElecto) );
    pcb_set_estimacion_anterior(pcbElecto,setearEstimacionAnteriorPCBelecto);
    } else {
        pcb_set_estimacion_anterior(pcbElecto, kernel_config_get_estimacion_inicial(kernelConfig)); 
    }
    

    return pcbElecto;
}
