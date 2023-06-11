#include <../include/memoria-estructuras.h>

extern t_memoria_config* memoriaConfig;
void* memoriaPrincipal;
uint32_t tamActualMemoria;
extern Segmento* segCompartido;

/*
inicializar mp
crear segmento
crear seg0 compartido
crear tala de segmentos
agregar un segmento
eliminar un segmento
*/

int segmento_get_id(Segmento* un_segmento){
    return un_segmento->segmento_id;
}

void segmento_set_id(Segmento* un_segmento, int id_segment){
    un_segmento->segmento_id = id_segment;
}

uint32_t* segmento_get_limite(Segmento* un_segmento){
    return un_segmento->limite;
}

void segmento_set_limite(Segmento* un_segmento, uint32_t* limite){
    un_segmento->limite = limite;
}

uint32_t* segmento_get_base(Segmento* un_segmento){
    return un_segmento->base;
}

void segmento_set_base(Segmento* un_segmento, uint32_t* base){
    un_segmento->base = base;       //habria que hacer malloc???
}

int segmento_get_socket(Segmento* un_segmento){
    return un_segmento->socket; 
}

void segmento_set_socket(Segmento* un_segmento, int socket){
    un_segmento->socket = socket;
}

int segmento_get_bit_validez(Segmento* un_segmento){
    return un_segmento->validez; 
}

void segmento_set_bit_validez(Segmento* un_segmento, int validezValor){
    un_segmento->validez = validezValor;
}

void inicializar_memoria(){
    int tamanioMP = (int) memoria_config_get_tamanio_memoria(memoriaConfig); 
    memoriaPrincipal = malloc(tamanioMP);
    tamActualMemoria = tamanioMP;
}

Segmento* crear_segmento(int tamSegmento){
    Segmento* this = malloc(sizeof(*this)); //no seria sizeof(limite)?? que pasa si limite es muy chico, no puedo guardar id_segmento...
    this->segmento_id = -1;
    this->base = NULL;
    segmento_set_limite(this, tamSegmento);
    this->socket = -1;
    this->validez = -1;

    return this;
}
void inicializar_estructuras(){
    inicializar_memoria();
    segCompartido = crear_segmento(memoria_config_get_tamanio_segmento_0(memoriaConfig));
}



/*
primero voy a crear segmentos y asignarlos de manera contigua, sin importar el algoritmo
una vez que implemente y pueda chequearlo recien ahi busco hacer realloc por si se agrandan los segmentos
y cuando tenga los algoritmos vere compactaion
*/