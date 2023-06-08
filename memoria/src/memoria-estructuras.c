#include <../include/memoria-estructuras.h>
t_memoria_config* memoriaConfig;
/*
inicializar mp
crear segmento
crear seg0 compartido
crear tala de segmentos
agregar un segmento
eliminar un segmento
*/

void inicializarMemoria(){
    int tamanioMP = (int) memoria_config_get_tamanio_memoria(memoriaConfig); 
    memoriaPrincipal = malloc(tamanioMP);
}

Segmento* crearSegmento(int idSegmento, uint32_t base, uint32_t limite){
    Segmento* segmento = malloc(sizeof(*segmento)); //no seria sizeof(limite)?? que pasa si limite es muy chico, no puedo guardar id_segmento...
    segmento->segmento_id = idSegmento;
    segmento->base = base;
    segmento->limite = limite;
    segmento->validez = 1;

    return segmento;
}

void inicializar_estructuras(){
    tabla_de_segmentos = list_create();
    Segmento* segCompartido = crearSegmento(0,0,memoria_config_get_tamanio_segmento_0(memoriaConfig));
    list_add(tabla_de_segmentos, segCompartido);
}

void eliminar_segmento(Segmento* segABorrar){
    //deberia buscar con el id_segmento en el void* y borrarlo
    segABorrar->validez = -1;
}

/*
primero voy a crear segmentos y asignarlos de manera contigua, sin importar el algoritmo
una vez que implemente y pueda chequearlo recien ahi busco hacer realloc por si se agrandan los segmentos
y cuando tenga los algoritmos vere compactaion
*/