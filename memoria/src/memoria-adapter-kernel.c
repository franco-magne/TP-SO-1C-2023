#include <../include/memoria-adapter-kernel.h>

t_segmento* segmento_kernel_create(uint32_t pid){
    t_segmento* aux = malloc(sizeof(*aux));
    aux->pid = pid;
    aux->id_de_segmento = 0;
    aux->base_del_segmento = -1;
    aux->tamanio_de_segmento = -1;
    aux->victima = false;

    return aux;
}

t_segmento* adapter_segmento_memoria_kernel(Segmento* segmentoAAdaptar){

    t_segmento* segmentoKernel = segmento_kernel_create(segmentoAAdaptar->pid);

    segmentoKernel->id_de_segmento = segmento_get_id(segmentoAAdaptar);
    segmentoKernel->base_del_segmento = segmento_get_base(segmentoAAdaptar);
    segmentoKernel->tamanio_de_segmento = segmento_get_tamanio(segmentoAAdaptar);
    segmentoKernel->victima = false;
    return segmentoKernel;
}

bool segmentos_del_mismo_pid(Segmento* unSegmento, Segmento* otroSegmento){
    return segmento_get_pid(unSegmento) == segmento_get_pid(otroSegmento);

}


