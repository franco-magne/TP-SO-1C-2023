#include <../include/memoria-estructuras.h>

extern t_memoria_config* memoriaConfig;
void* memoriaPrincipal;
uint32_t tamActualMemoria;
extern Segmento* segCompartido; // tipo lista enlazada (obligatorio)
extern t_list* listaDeProcesos;

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

int segmento_get_pid(Segmento* un_segmento){
    return un_segmento->pid; 
}

void segmento_set_pid(Segmento* un_segmento, int pid){
    un_segmento->pid = pid;
}

int segmento_get_bit_validez(Segmento* un_segmento){
    return un_segmento->validez; 
}

void segmento_set_bit_validez(Segmento* un_segmento, int validezValor){
    un_segmento->validez = validezValor;
}

Segmento* crear_segmento(int tamSegmento){
    Segmento* this = malloc(sizeof(*this)); //no seria sizeof(limite)?? que pasa si limite es muy chico, no puedo guardar id_segmento...
    this->segmento_id = -1;
    this->base = NULL;
    segmento_set_limite(this, tamSegmento);
    this->pid = -1;
    this->validez = -1;

    return this;
}
////////////deberia estar en memoria
void inicializar_memoria(){
    int tamanioMP = (int) memoria_config_get_tamanio_memoria(memoriaConfig); 
    memoriaPrincipal = malloc(tamanioMP);
    tamActualMemoria = tamanioMP;
}

void inicializar_estructuras(){
    segCompartido = crear_segmento(memoria_config_get_tamanio_segmento_0(memoriaConfig));
    listaDeProcesos = list_create();
}
//----------------------------------------------

Procesos* crear_proceso(int pid){ //la tabla de segmentos del proceso de pid: pid
    Procesos* this = malloc(sizeof(*this));
    this->tablaDeSegmentos = list_create();
    this->pid = pid;
    
    //Inicializa la tabla de segmentos
    list_add(this->tablaDeSegmentos, segCompartido);
    //hay que restar tamActualMemoria
    return this;
}





/*
primero voy a crear segmentos y asignarlos de manera contigua, sin importar el algoritmo
una vez que implemente y pueda chequearlo recien ahi busco hacer realloc por si se agrandan los segmentos
y cuando tenga los algoritmos vere compactaion
*/