#include <../include/memoria-adapter-kernel.h>

t_segmento* segmento_kernel_create(){
    t_segmento* aux = malloc(sizeof(*aux));

    aux->id_de_segmento = 0;
    aux->base_del_segmento = -1;
    aux->tamanio_de_segmento = -1;
    aux->victima = false;

    return aux;
}

t_segmento* adapter_segmento_memoria_kernel(Segmento* segmentoAAdaptar){

    t_segmento* segmentoKernel = segmento_kernel_create();

    segmentoKernel->id_de_segmento = segmento_get_id(segmentoAAdaptar);
    segmentoKernel->base_del_segmento = segmento_get_base(segmentoAAdaptar);
    segmentoKernel->tamanio_de_segmento = segmento_get_tamanio(segmentoAAdaptar);
    segmentoKernel->victima = false;
    return segmentoKernel;
}

bool segmentos_del_mismo_pid(Segmento* unSegmento, Segmento* otroSegmento){
    return segmento_get_pid(unSegmento) == segmento_get_pid(otroSegmento);

}

void buffer_pack_segmento(t_buffer* buffer, t_segmento* segmento) {
    buffer_pack(buffer, &(segmento->id_de_segmento), sizeof(segmento->id_de_segmento));
    buffer_pack(buffer, &(segmento->base_del_segmento), sizeof(segmento->base_del_segmento));
    buffer_pack(buffer, &(segmento->tamanio_de_segmento), sizeof(segmento->tamanio_de_segmento));
    buffer_pack(buffer, &(segmento->victima), sizeof(segmento->victima));
}

void buffer_pack_segmento_list(t_buffer* buffer, t_list* lista_segmentos) {
    int cantidad_segmentos = list_size(lista_segmentos);
    buffer_pack(buffer, &cantidad_segmentos, sizeof(cantidad_segmentos));

    for(int i = 0; i<list_size(lista_segmentos); i++){
       t_segmento* segmento = list_get(lista_segmentos,i);
       buffer_pack_segmento(buffer,segmento);
    }

}

