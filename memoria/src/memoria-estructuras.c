#include <../include/memoria-estructuras.h>

extern t_log *memoriaLogger;
extern t_memoria_config* memoriaConfig;

extern Segmento* segCompartido; // tipo lista enlazada (obligatorio)

extern t_list* listaDeSegmentos;
extern uint32_t tamActualMemoria;
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

uint32_t segmento_get_limite(Segmento* un_segmento){
    return un_segmento->limite;
}

void segmento_set_limite(Segmento* un_segmento, uint32_t limite){
    un_segmento->limite = limite;
}

uint32_t segmento_get_tamanio(Segmento* un_segmento){
    return un_segmento->tamanio;
}

void segmento_set_tamanio(Segmento* un_segmento, uint32_t tamanio){
    un_segmento->tamanio = tamanio;
}

uint32_t segmento_get_base(Segmento* un_segmento){
    return un_segmento->base;
}

void segmento_set_base(Segmento* un_segmento, uint32_t base){
    un_segmento->base = base;       
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

char* segmento_get_contenido(Segmento* un_segmento){
    return un_segmento->contenido; 
}

void segmento_set_contenido(Segmento* un_segmento, char* contenido){
    un_segmento->contenido = contenido;
}


bool es_el_segmento_victima_pid(Segmento* element, Segmento* otro_segmento) {
   return element->pid == otro_segmento->pid;
}

t_list* obtener_tabla_de_segmentos_por_pid(int pid){
    t_list* tablaDeSegdelProceso = list_create();
    Segmento* aux = crear_segmento(-1);
    segmento_set_pid(aux,pid);
    tablaDeSegdelProceso = list_filter_ok(listaDeSegmentos, es_el_segmento_victima_pid,aux);
    free(aux);
    return tablaDeSegdelProceso;
}

bool es_el_segmento_por_BASE(Segmento* element, Segmento* segVictima){
    return element->base ==  segVictima->base;
}

Segmento* crear_segmento(int tamSegmento){
    Segmento* this = malloc(sizeof(*this)); //no seria sizeof(limite)?? que pasa si limite es muy chico, no puedo guardar id_segmento...
    this->segmento_id = -1;
    this->base = -1;
    segmento_set_tamanio(this, tamSegmento);
    this->pid = -1;
    this->validez = -1;
    this->contenido = NULL;

    return this;
}

bool es_el_segmento_victima_id(Segmento* unSegmento, Segmento* otroSegmento) {
   return (unSegmento->segmento_id == otroSegmento->segmento_id) && (unSegmento->pid == otroSegmento->pid);
}

Segmento* obtener_segmento_por_id(int pid_victima, int id_victima){
    Segmento* aux1 = crear_segmento(-1);
    segmento_set_id(aux1, id_victima);
    segmento_set_pid(aux1, pid_victima);

    uint32_t index = list_get_index(listaDeSegmentos, es_el_segmento_victima_id, aux1);
    printf("Valor de index <%i>", index);
    Segmento* aux2 = list_get(listaDeSegmentos, index);
    free(aux1);
    
    return aux2;
}

Segmento* obtener_segmento_por_BASE(uint32_t base_segmento){
    Segmento* aux1 = crear_segmento(-1);
    segmento_set_base(aux1, base_segmento);

    uint32_t index = list_get_index(listaDeSegmentos, es_el_segmento_por_BASE, aux1);
    Segmento* aux2 = list_get(listaDeSegmentos, index);
    free(aux1);
    return aux2;
}

void modificarSegmento(uint32_t baseSegmento, Segmento* segNuevo){
    Segmento* aux1 = crear_segmento(-1);
    segmento_set_base(aux1, baseSegmento);

    uint32_t index = list_get_index(listaDeSegmentos, es_el_segmento_por_BASE, aux1);
    list_replace(listaDeSegmentos, index, segNuevo);
    free(aux1);
}

Segmento* desencolar_segmento_por_id(int pid_segmento, int id_segmento){
    if(list_is_empty(listaDeSegmentos)){
      exit(EXIT_FAILURE);
    }
    else{
        pthread_mutex_lock(&mutexMemoriaEstruct);
        return list_remove(listaDeSegmentos, obtener_segmento_por_id(pid_segmento, id_segmento));
        pthread_mutex_unlock(&mutexMemoriaEstruct);
    }
}

Segmento* desencolar_segmento_primer_segmento_atomic(){
    if(list_is_empty(listaDeSegmentos)){
      exit(EXIT_FAILURE);
    }
    else{
        pthread_mutex_lock(&mutexMemoriaEstruct);
        return list_remove(listaDeSegmentos, 1);
        pthread_mutex_unlock(&mutexMemoriaEstruct);
    }
}

void encolar_segmento_atomic(Segmento* targetSegmento) 
{
  pthread_mutex_lock(&mutexMemoriaEstruct);
  list_add(listaDeSegmentos, targetSegmento);
  pthread_mutex_unlock(&mutexMemoriaEstruct);
}

void encolar_segmento_atomic_en_tablaDada(t_list* tablaDada, Segmento* targetSegmento) 
{
  pthread_mutex_lock(&mutexMemoriaEstruct);
  list_add(tablaDada, targetSegmento);
  pthread_mutex_unlock(&mutexMemoriaEstruct);
}

void sumar_memoriaRecuperada_a_tamMemoriaActual(uint32_t tamMemorRecuperada){
    pthread_mutex_lock(&mutexTamMemoriaActual);
    tamActualMemoria += tamMemorRecuperada;
    log_info(memoriaLogger,BOLD CYAN  "TAMAÑO ACTUAL MEMORIA "RESET BOLD GREEN" <%i>", tamActualMemoria);

    pthread_mutex_unlock(&mutexTamMemoriaActual);
}

void restar_a_tamMemoriaActual(uint32_t memoriaARestar){
    pthread_mutex_lock(&mutexTamMemoriaActual);
    tamActualMemoria -= memoriaARestar;
    log_info(memoriaLogger,BOLD CYAN  "TAMAÑO ACTUAL MEMORIA "RESET BOLD RED" <%i>", tamActualMemoria);
    pthread_mutex_unlock(&mutexTamMemoriaActual);
}

bool puedo_crear_proceso_o_segmento(uint32_t tamanio){
    pthread_mutex_lock(&mutexTamMemoriaActual);
    bool condicion = (tamanio <= tamActualMemoria);
    pthread_mutex_unlock(&mutexTamMemoriaActual);
    return condicion;
}


////////////deberia estar en memoria.c
void inicializar_memoria(){
    
    


}

void inicializar_estructuras(){
    
    
}
//----------------------------------------------

