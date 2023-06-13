#include "../include/pcb.h"



t_pcb* pcb_create(uint32_t pid) 
{
   printf("Creando nuevo PCB con PID %d\n", pid); 
    t_pcb* this = malloc(sizeof(*this));
   if (this == NULL) {
      printf("Error al asignar memoria para el PCB\n");
      exit(EXIT_FAILURE);
   }
   this->pid = pid;
   this->estadoActual = NEW;
   this->estadoAnterior = NEW;
   this->programCounter = 0;
   this->instruccionesBuffer = NULL;
   this->registros = registros_cpu_create();
   this->tiempoIO = 0;
   this->recursoUtilizado = NULL;
   this->rafaga_actual = -1;
   this->rafaga_anterior = -1;
   this->listaDeSegmento = list_create();
   return this;
}

//////////////////////////////////// SEGMENTO /////////////////////////////////////////

t_segmento* segmento_create(uint32_t id_de_segmento, uint32_t tamanio_de_segmento){
    t_segmento* this = malloc(sizeof(*this));
    this->id_de_segmento = id_de_segmento;
    this->tamanio_de_segmento = tamanio_de_segmento;
    this->victima = true;
    return this;
}

void segmento_destroy(t_segmento* this){
    free(this);
}

uint32_t segmento_get_id_de_segmento(t_segmento* this){
    return this->id_de_segmento;
}

uint32_t segmento_get_tamanio_de_segmento(t_segmento* this){
    return this->tamanio_de_segmento;
}

bool segmento_get_victima(t_segmento* this){
    return this->victima;
}

void segmento_set_victima(t_segmento* this, bool cambioEstado){
    this->victima = cambioEstado;
}

bool es_el_segmento_victima(t_segmento* element, t_segmento* target) {
    if(element->victima)
    return true;
    return false;
}

t_segmento* enviar_segmento_a_memoria(t_pcb* this, uint8_t header) {
    t_segmento* aux1 = segmento_create(-1, -1);
    uint32_t index = list_get_index(pcb_get_lista_de_segmentos(this), es_el_segmento_victima, aux1);
    t_segmento* aux2 = NULL;
    
    if (index != -1) {
        if(header == HEADER_create_segment){
        aux2 = list_get(pcb_get_lista_de_segmentos(this), index); // Aca no lo queremos quitar, lo leemos y le seteamos el valor asi deja de ser victima
        segmento_set_victima(aux2, false);
        list_replace(pcb_get_lista_de_segmentos(this), index, aux2);
        }
        else if(header == HEADER_delete_segment){
        aux2 = list_remove(pcb_get_lista_de_segmentos(this), index); // Aca si porque lo removemos a ese segmento
        } 

    }

    segmento_destroy(aux1);
    
    return aux2;
}

bool es_el_segmento_victima_id(t_segmento* element, t_segmento* target) {
   return element->id_de_segmento == target ->id_de_segmento;
}

void modificar_victima_lista_segmento(t_pcb* this, uint32_t id_victima){
    t_segmento* aux1 = segmento_create(id_victima, -1);
    uint32_t index = list_get_index(pcb_get_lista_de_segmentos(this), es_el_segmento_victima_id, aux1);
    if (index != -1) {
        t_segmento* aux2 = list_get(pcb_get_lista_de_segmentos(this), index);
        segmento_set_victima(aux2, true);
        list_replace(pcb_get_lista_de_segmentos(this), index, aux2);
        segmento_destroy(aux1); // Liberar segmento anteriormente creado
    } else {
        segmento_destroy(aux1); // Liberar segmento si no se encontró en la lista
    }
}
//////////////////////////////////////////////////////////////////////////////////////


//////////////////////// GETTERS /////////////////////

t_registros_cpu* pcb_get_registros_cpu(t_pcb* this)
{
    return this->registros;
}

uint32_t pcb_get_program_counter(t_pcb* this)
{
    return this->programCounter;
}

uint32_t pcb_get_pid(t_pcb* this) 
{
    return this->pid;
}

t_buffer* pcb_get_instrucciones_buffer(t_pcb* this)
{   
    return this->instruccionesBuffer;
}

t_nombre_estado pcb_get_estado_actual(t_pcb* this){
    return this->estadoActual;
}

t_nombre_estado pcb_get_estado_anterior(t_pcb* this){
    return this->estadoAnterior;
}

uint32_t pcb_get_tiempoIO(t_pcb* this) 
{
    return this->tiempoIO;
}

char* pcb_get_recurso_utilizado(t_pcb* this){
    return this->recursoUtilizado;
}

struct timespec pcb_get_tiempo_en_ready(t_pcb* this){
    return this->tiempo_ready;
}

double pcb_get_rafaga_anterior(t_pcb* this){
    return this->rafaga_anterior;
}
double pcb_get_rafaga_actual(t_pcb* this){
   return this->rafaga_actual;
}


t_list* pcb_get_lista_de_segmentos(t_pcb* this){
    return this->listaDeSegmento;
}
/////////////////////// SETTER ////////////////////////

void pcb_set_program_counter(t_pcb* this, uint32_t programCounter) 
{
    this->programCounter = programCounter;
}

void pcb_set_instructions_buffer(t_pcb* this, t_buffer* instructionsBuffer) 
{
    this->instruccionesBuffer = instructionsBuffer;
}

void pcb_set_pid(t_pcb* this, uint32_t pid) 
{
   this->pid = pid;
}

void pcb_set_estado_actual(t_pcb* this, uint32_t estadoActual) 
{
   this->estadoActual = estadoActual;
}

void pcb_set_estado_anterior(t_pcb* this, uint32_t estadoAnterior) 
{
   this->estadoAnterior = estadoAnterior;
}

void pcb_set_registro_ax_cpu(t_pcb* this, uint32_t registro)
{
    this->registros->registroAx = registro;
}

void pcb_set_registro_bx_cpu(t_pcb* this, uint32_t registro)
{
    this->registros->registroBx = registro;
}

void pcb_set_registro_cx_cpu(t_pcb* this, uint32_t registro)
{
    this->registros->registroCx = registro;
}

void pcb_set_registro_dx_cpu(t_pcb* this, uint32_t registro)
{
    this->registros->registroDx = registro;
}

void pcb_set_tiempoIO(t_pcb* this, uint32_t tiempoIO)
{
    this->tiempoIO = tiempoIO;
}

void pcb_set_recurso_utilizado(t_pcb* this, char* recurso)
{
    this->recursoUtilizado = recurso;
}

bool pcb_es_este_pcb_por_pid(void* unPcb, void* otroPcb) 
{
    return pcb_get_pid((t_pcb*)unPcb) == pcb_get_pid((t_pcb*)otroPcb);
}

void pcb_set_tiempo_en_ready(t_pcb* this, struct timespec tiempo_ready){
    this->tiempo_ready = tiempo_ready;
}

void pcb_set_rafaga_anterior(t_pcb* this, double rafaga){
    this->rafaga_anterior = rafaga;
}

void pcb_set_rafaga_actual(t_pcb* this, double rafaga){
    this->rafaga_actual = rafaga;
}


void pcb_set_lista_de_segmentos(t_pcb* this, t_segmento* unSegmento){
   list_add(this->listaDeSegmento,unSegmento);
}

