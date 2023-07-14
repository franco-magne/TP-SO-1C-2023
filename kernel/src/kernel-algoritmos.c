#include <../include/kernel-algoritmos.h>

extern t_kernel_config* kernelConfig;


////////////////////////////////// FIFO /////////////////////////////

t_pcb* elegir_pcb_segun_fifo(t_estado* estado)
{
    return estado_desencolar_primer_pcb_atomic(estado);
}


//////////////////////////////// HHRN ////////////////////////////////
static double calcularHRRN(double tiempoDeEspera , double rafagaEstimadaProxima) {
    return (1 + (tiempoDeEspera / rafagaEstimadaProxima));

}

static double calcularEstimacionProxima(double estimacionAnterior, double realAnterior) {
    double alfa = kernelConfig->HRRN_ALFA;

    return alfa * estimacionAnterior + (1 - alfa) * realAnterior;
}

double calcular_siguiente_estimacion(t_pcb* pcb) {
  
struct timespec tiempoFinalSpec;
time_t tiempoActual = time(NULL);
struct timespec tiempoEnReadySpec;
struct timespec tiempoDeEsperaSpec;
double tiempoDeEspera = 0;
double resultado = 0;
double resultadoEstimacionProxima = 0;

if(pcb_get_estimacion_anterior(pcb) == 0){
pcb_set_estimacion_anterior(pcb, kernel_config_get_estimacion_inicial(kernelConfig));
pcb_set_rafaga_anterior(pcb,0);
}

tiempoFinalSpec.tv_sec = tiempoActual;
tiempoFinalSpec.tv_nsec = 0;

tiempoEnReadySpec = pcb_get_tiempo_en_ready(pcb);

tiempoDeEsperaSpec.tv_sec = tiempoFinalSpec.tv_sec - tiempoEnReadySpec.tv_sec;
tiempoDeEsperaSpec.tv_nsec = tiempoFinalSpec.tv_nsec - tiempoEnReadySpec.tv_nsec;
if (tiempoDeEsperaSpec.tv_nsec < 0) {
    tiempoDeEsperaSpec.tv_sec--;
    tiempoDeEsperaSpec.tv_nsec += 1000000000;
}

tiempoDeEspera = tiempoDeEsperaSpec.tv_sec;

resultadoEstimacionProxima = calcularEstimacionProxima( pcb_get_estimacion_anterior(pcb) , pcb_get_rafaga_anterior(pcb) );

resultado = calcularHRRN(tiempoDeEspera, resultadoEstimacionProxima);

return resultado;

}

static t_pcb* mayor_ratio(t_pcb* unPcb, t_pcb* otroPcb) {
    double unaRafagaRestante = calcular_siguiente_estimacion(unPcb);
    double otraRafagaRestante = calcular_siguiente_estimacion(otroPcb);
        printf(RED BOLD"PCB <%f> - PCB <%f> \n" RESET,unaRafagaRestante,otraRafagaRestante);
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
    
    setearEstimacionAnteriorPCBelecto =  calcularEstimacionProxima( pcb_get_estimacion_anterior(pcbElecto) , pcb_get_rafaga_anterior(pcbElecto) );
    pcb_set_estimacion_anterior(pcbElecto,setearEstimacionAnteriorPCBelecto);

    return pcbElecto;
}
