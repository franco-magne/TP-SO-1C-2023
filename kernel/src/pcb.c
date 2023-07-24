#include "../include/pcb.h"



t_pcb* pcb_create(uint32_t pid) 
{
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
   this->socketConsola = -1;
   this->tiempoIO = 0;
   this->recursoUtilizado = NULL;
   this->rafaga_actual = 0;
   this->rafaga_anterior = 0;
   this->estimacionActual = 0;
   this->listaDeSegmento = list_create();
   this->listaArchivosAbiertos = list_create();
   return this;
}


void pcb_destroy(t_pcb* self) 
{

    list_destroy(self->listaArchivosAbiertos);
    list_destroy(self->listaDeSegmento);
    buffer_destroy(self->instruccionesBuffer);
    free(self->recursoUtilizado);
    if(self->tiempo_ready != NULL){
    temporal_destroy(self->tiempo_ready);
    }
    free(self->registros);
    free(self);
}


//////////////////////////////////// SEGMENTO /////////////////////////////////////////


void segmento_destroy(t_segmento* this){
    free(this);
}

uint32_t segmento_get_id_de_segmento(t_segmento* this){
    return this->id_de_segmento;
}

uint32_t segmento_get_tamanio_de_segmento(t_segmento* this){
    return this->tamanio_de_segmento;
}

uint32_t segmento_get_base_de_segmento(t_segmento* this){
    return this->base_del_segmento;
}

bool segmento_get_victima(t_segmento* this){
    return this->victima;
}

void segmento_set_victima(t_segmento* this, bool cambioEstado){
    this->victima = cambioEstado;
}

uint32_t segmento_get_pid(t_segmento* this){
    return this->pid;
}

void segmento_set_pid(t_segmento* this, uint32_t pid){
    this->pid = pid;
}
///////////////////////////// FUNCIONES UTILITARIAS DEL SEGMENTO POSIBLE MIGRACION A KERNEL-ESTRUCTURA-SEGMENTO //////////////////

bool es_el_segmento_por_id(t_segmento* unSegmento, t_segmento* otroSegmento){
    return (unSegmento->id_de_segmento == otroSegmento->id_de_segmento) && (unSegmento->pid == otroSegmento->pid);
}

bool es_el_segmento_pid(t_segmento* unSegmento, t_segmento* otroSegmento){
    return unSegmento->pid == otroSegmento->pid;
}

bool es_el_segmento_victimaok(t_segmento* element) {
    if(element->victima)
    return true;
    return false;
}

t_segmento* segmento_victima(t_list* this) {
    t_segmento* aux2 = list_find(this, es_el_segmento_victimaok);
    return aux2;
}

uint32_t index_posicion_del_segmento_victima(t_list* this, uint32_t id, uint32_t pid){
    t_segmento* aux1 = segmento_create(id, -1);
    segmento_set_pid(aux1, pid);
    uint32_t index = list_get_index(this,es_el_segmento_por_id, aux1);
    segmento_destroy(aux1);
    return index;
}

void modificar_victima_lista_segmento(t_list* this, uint32_t id_victima, uint32_t pid, bool cambiovictima){
    uint32_t index = index_posicion_del_segmento_victima(this,id_victima,pid);
    if (index != -1) {
        t_segmento* aux2 = list_get(this, index);
        segmento_set_victima(aux2, cambiovictima);
        list_replace(this, index, aux2);
        
    } 
}

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

int pcb_get_socket_consola(t_pcb* this){
    return this->socketConsola;
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

t_temporal* pcb_get_tiempo_en_ready(t_pcb* this){
    return this->tiempo_ready;
}

double pcb_get_rafaga_anterior(t_pcb* this){
    return this->rafaga_anterior;
}
double pcb_get_estimacion_anterior(t_pcb* this){
   return this->estimacionActual;
}


t_list* pcb_get_lista_de_segmentos(t_pcb* this){
    return this->listaDeSegmento;
}

t_list* pcb_get_lista_de_archivos_abiertos(t_pcb* this){
    return this->listaArchivosAbiertos;
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

void pcb_set_socket_consola(t_pcb* this, int socket){
    this->socketConsola = socket;
}

void pcb_set_estado_actual(t_pcb* this, uint32_t estadoActual) 
{
   this->estadoActual = estadoActual;
}

void pcb_set_estado_anterior(t_pcb* this, uint32_t estadoAnterior) 
{
   this->estadoAnterior = estadoAnterior;
}

void pcb_set_registro_ax_cpu(t_pcb* this, char* registro)
{
    this->registros->registroAx = registro;
}

void pcb_set_registro_bx_cpu(t_pcb* this, char* registro)
{
    this->registros->registroBx = registro;
}

void pcb_set_registro_cx_cpu(t_pcb* this, char* registro)
{
    this->registros->registroCx = registro;
}

void pcb_set_registro_dx_cpu(t_pcb* this, char* registro)
{
    this->registros->registroDx = registro;
}

void pcb_set_registro_eax_cpu(t_pcb* this, char* registro)
{
    this->registros->registroEAx = registro;
}

void pcb_set_registro_ebx_cpu(t_pcb* this, char* registro)
{
    this->registros->registroEBx = registro;
}

void pcb_set_registro_ecx_cpu(t_pcb* this, char* registro)
{
    this->registros->registroECx = registro;
}

void pcb_set_registro_edx_cpu(t_pcb* this, char* registro)
{
    this->registros->registroEDx = registro;
}

void pcb_set_registro_rax_cpu(t_pcb* this, char* registro)
{
    this->registros->registroRAx = registro;
}

void pcb_set_registro_rbx_cpu(t_pcb* this, char* registro)
{
    this->registros->registroRBx = registro;
}

void pcb_set_registro_rcx_cpu(t_pcb* this, char* registro)
{
    this->registros->registroRCx = registro;
}

void pcb_set_registro_rdx_cpu(t_pcb* this, char* registro)
{
    this->registros->registroRDx = registro;
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
//    this->tiempo_ready = tiempo_ready;
}

void pcb_set_rafaga_anterior(t_pcb* this, double rafaga){
    this->rafaga_anterior = rafaga;
}

void pcb_set_estimacion_anterior(t_pcb* this, double rafaga){
    this->estimacionActual = rafaga;
}


void pcb_set_lista_de_segmentos(t_pcb* this, t_list* listaDeSegmento){
   this->listaDeSegmento = listaDeSegmento;
}

void pcb_add_lista_de_archivos(t_pcb* this,t_pcb_archivo* unArchivo ){
    list_add(this->listaArchivosAbiertos,unArchivo);
}
