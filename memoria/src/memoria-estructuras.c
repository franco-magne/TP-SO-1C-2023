#include <../include/memoria-estructuras.h>

extern t_memoria_config* memoriaConfig;
void* memoriaPrincipal;
uint32_t tamActualMemoria;
extern Segmento* segCompartido; // tipo lista enlazada (obligatorio)
extern t_list* listaDeProcesos;
extern pthread_mutex_t* mutexTamMemoriaActual;
pthread_mutex_t* mutexMemoriaEstruct; //seria mutexMemoriaData

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

uint32_t segmento_get_tamanio(Segmento* un_segmento){
    return un_segmento->tamanio;
}

void segmento_set_tamanio(Segmento* un_segmento, uint32_t tamanio){
    un_segmento->tamanio = tamanio;
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

int segmento_get_contenido(Segmento* un_segmento){
    return un_segmento->contenido; 
}

void segmento_set_contenido(Segmento* un_segmento, char* contenido){
    un_segmento->contenido = contenido;
}

Segmento* crear_segmento(int tamSegmento){
    Segmento* this = malloc(sizeof(*this)); //no seria sizeof(limite)?? que pasa si limite es muy chico, no puedo guardar id_segmento...
    this->segmento_id = -1;
    this->base = NULL;
    segmento_set_limite(this, tamSegmento);
    this->pid = -1;
    this->validez = -1;
    this->contenido = NULL;

    return this;
}
////////////deberia estar en memoria.c
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
    pthread_mutex_lock(&mutexTamMemoriaActual);
    list_add(this->tablaDeSegmentos, segCompartido);
    tamActualMemoria -= memoria_config_get_tamanio_segmento_0(memoriaConfig);
    pthread_mutex_unlock(&mutexTamMemoriaActual);

    return this;
}

bool es_el_proceso_victima_id(Procesos* element, Procesos* target) {
   return element->pid == target->pid;
}
bool es_el_segmento_victima_id(Segmento* element, int id_segmento) {
   return element->segmento_id == id_segmento;
}

Procesos* obtener_proceso_por_pid(int pid_victima){
    Procesos* aux1 = crear_proceso(pid_victima);
    uint32_t index = list_get_index(listaDeProcesos, es_el_proceso_victima_id, aux1);
    Procesos* aux2 = list_get(listaDeProcesos, index);
    free(aux1);
    
    return aux2;
}



Segmento* obtener_segmento_por_id(Procesos* proceso, int id_victima){
    Segmento* aux1 = crear_segmento(-1);
    segmento_set_id(aux1, id_victima);

    uint32_t index = list_get_index(proceso->tablaDeSegmentos, es_el_segmento_victima_id, aux1);
    Segmento* aux2 = list_get(proceso->tablaDeSegmentos, index);
    free(aux1);
    
    return aux2;
}

Segmento* desencolar_segmento_por_id(Procesos* proceso, int id_segmento){
    if(list_is_empty(proceso->tablaDeSegmentos)){
      exit(EXIT_FAILURE);
    }
    else{
        pthread_mutex_lock(&mutexMemoriaEstruct);
        return list_remove(proceso->tablaDeSegmentos, obtener_segmento_por_id(proceso, id_segmento));
        pthread_mutex_unlock(&mutexMemoriaEstruct);
    }
}

Segmento* desencolar_segmento_primer_segmento_atomic(Procesos* proceso){
    if(list_is_empty(proceso->tablaDeSegmentos)){
      exit(EXIT_FAILURE);
    }
    else{
        pthread_mutex_lock(&mutexMemoriaEstruct);
        return list_remove(proceso->tablaDeSegmentos, 1);
        pthread_mutex_unlock(&mutexMemoriaEstruct);
    }
}

void sumar_memoriaRecuperada_a_tamMemoriaActual(uint32_t tamMemorRecuperada){
    pthread_mutex_lock(&mutexTamMemoriaActual);
    tamActualMemoria += tamMemorRecuperada;
    pthread_mutex_unlock(&mutexTamMemoriaActual);
}

void liberar_tabla_segmentos(int pid){
    Procesos* unProceso = obtener_proceso_por_pid(pid);
    pthread_mutex_lock(&mutexMemoriaEstruct);
    tamActualMemoria += list_size(unProceso->tablaDeSegmentos); //habria q hacer una sumatoria del "limite" de todos los segmentos
    pthread_mutex_unlock(&mutexMemoriaEstruct);
    list_destroy(unProceso->tablaDeSegmentos);
}
void encolar_segmento_atomic(t_list* tablaDeSegmentos, Segmento* targetSegmento) 
{
  pthread_mutex_lock(&mutexMemoriaEstruct);
  list_add(tablaDeSegmentos, targetSegmento);
  pthread_mutex_unlock(&mutexMemoriaEstruct);
}

/*Segmento* estado_desencolar_primer_segmento(Segmento* this){
    if(list_is_empty(this)){
      exit(EXIT_FAILURE);
    }
    else{
      return list_remove(estado_get_list(this), 1); //es 1 xq el Seg0 no se borra nunca
    }
}*/

/*
bool es_el_segmento_victima_id(t_segmento* element, t_segmento* target) {
   return element->id_de_segmento == target ->id_de_segmento;
}

void modificar_victima_lista_segmento(t_pcb* this, uint32_t id_victima){
    t_segmento* aux1 = segmento_create(id_victima, -1);
    uint32_t index = list_get_index(pcb_get_lista_de_segmentos(this), es_el_segmento_victima_id, aux1);
    if (index != -1) {
        t_segmento* aux2 = list_get(pcb_get_lista_de_segmentos(this), index);
        if(aux2 contiene el segmento ta ta ta)

    } else {
        segmento_destroy(aux1); // Liberar segmento si no se encontró en la lista
    }
} 
void modificar_proceso(pid)  { t_proceso aux= proceso_Create(pid) ; int index = list_get_index(lista_deprocesos, comparar_pid, aux)
t_proce aux2 = list_get(lista_procesos,index); -> Aca te lo traes a ese proceso


*/

/*
primero voy a crear segmentos y asignarlos de manera contigua, sin importar el algoritmo
una vez que implemente y pueda chequearlo recien ahi busco hacer realloc por si se agrandan los segmentos
y cuando tenga los algoritmos vere compactaion
*/