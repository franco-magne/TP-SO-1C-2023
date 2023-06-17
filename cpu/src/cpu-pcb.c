#include <../include/cpu-pcb.h>

t_cpu_pcb* cpu_pcb_create (uint32_t pid, uint32_t programCounter, t_registros_cpu* registrosCpu)
{
    t_cpu_pcb* this = malloc(sizeof(*this));
    this->pid = pid;
    this->programCounter = programCounter;
    this->registrosCpu = registrosCpu;
    this->instrucciones = NULL;
    this->tiempoIO = 0;
    this->recursoUtilizado = NULL;
    this->tamanio_de_segmento = -1;
    this->id_de_segmento = -1;
    this->nombreArchivo = NULL;

    return this;
}

void cpu_pcb_destroy(t_cpu_pcb* this) 
{
    // destroy_instructions_list(this->instrucciones);
    free(this->registrosCpu);

    free(this);
}
///////////////////// COMIENZO DE RECURSOS ///////////////////
recurso* recursos_inicializar(){
    recurso* aux = malloc(sizeof(*aux));
    aux->recursoUtilizado = NULL;
    aux->tiempoIO = -1;

    return aux;
}



////////////////// GETTERS Y SETTERS ////////////////


uint32_t cpu_pcb_get_pid(t_cpu_pcb* this) 
{
    return this->pid;
}

void cpu_pcb_set_pid(t_cpu_pcb* this, uint32_t pid) 
{
    this->pid = pid;
}

uint32_t cpu_pcb_get_program_counter(t_cpu_pcb* this) 
{
    return this->programCounter;
}

void cpu_pcb_set_program_counter(t_cpu_pcb* this, uint32_t programCounter) 
{
    this->programCounter = programCounter;
}

t_list* cpu_pcb_get_instrucciones(t_cpu_pcb* this) 
{
    return this->instrucciones;
}

void cpu_pcb_set_instrucciones(t_cpu_pcb* this, t_list* instrucciones) 
{
    this->instrucciones = instrucciones;
}

t_registros_cpu* cpu_pcb_get_registros(t_cpu_pcb* this)
{
    return this->registrosCpu;
}

void cpu_pcb_set_registros(t_cpu_pcb* this, t_registros_cpu* registrosCpu)
{
    this->registrosCpu = registrosCpu;
}

uint32_t cpu_pcb_get_tiempoIO(t_cpu_pcb* this) 
{
    return this->tiempoIO;
}

void cpu_pcb_set_tiempoIO(t_cpu_pcb* this, uint32_t tiempoIO)
{
    this->tiempoIO = tiempoIO;
}

void cpu_pcb_set_recurso_utilizar(t_cpu_pcb* this, char* recurso)
{
    this->recursoUtilizado = recurso;
}

char* cpu_pcb_get_recurso_utilizar(t_cpu_pcb* this)
{
    this->recursoUtilizado;
}

void cpu_pcb_set_tamanio_de_segmento(t_cpu_pcb* this, uint32_t tamanio){
    this->tamanio_de_segmento = tamanio;
}

uint32_t cpu_pcb_get_tamanio_de_segmento(t_cpu_pcb* this){
    return this->tamanio_de_segmento;
}

void cpu_pcb_set_id_de_segmento(t_cpu_pcb* this, uint32_t id){
    this->id_de_segmento = id;
}

uint32_t cpu_pcb_get_id_de_segmento(t_cpu_pcb* this){
    return this->id_de_segmento;
}

char* cpu_pcb_get_nombre_archivo(t_cpu_pcb* this){
    return this->nombreArchivo;
}

void cpu_pcb_set_nombre_archivo(t_cpu_pcb* this, char* nombreArchivo){
    this->nombreArchivo = nombreArchivo;
}

////////////////////////////  Registros 4bits  /////////////////////////////////////////
char*  cpu_pcb_get_registro_ax(t_cpu_pcb* this)
{
    return cpu_pcb_get_registros(this)->registroAx;

}

char*  cpu_pcb_get_registro_bx(t_cpu_pcb* this)
{
    return cpu_pcb_get_registros(this)->registroBx;
}

char*  cpu_pcb_get_registro_cx(t_cpu_pcb* this)
{
    return cpu_pcb_get_registros(this)->registroCx;
}

char*  cpu_pcb_get_registro_dx(t_cpu_pcb* this)
{
    return cpu_pcb_get_registros(this)->registroDx;
}

void cpu_pcb_set_registro_ax(t_cpu_pcb* this, char*  registro)
{
    cpu_pcb_get_registros(this)->registroAx = registro;
}

void cpu_pcb_set_registro_bx(t_cpu_pcb* this, char*  registro)
{
    cpu_pcb_get_registros(this)->registroBx = registro;
}

void cpu_pcb_set_registro_cx(t_cpu_pcb* this, char*  registro)
{
    cpu_pcb_get_registros(this)->registroCx = registro;
}

void cpu_pcb_set_registro_dx(t_cpu_pcb* this, char*  registro)
{
    cpu_pcb_get_registros(this)->registroDx = registro;
}

////////////////////////////  Registros 8bits  /////////////////////////////////////////
uint32_t cpu_pcb_get_registro_eax(t_cpu_pcb* this)
{
    return cpu_pcb_get_registros(this)->registroEAx;

}

uint32_t cpu_pcb_get_registro_ebx(t_cpu_pcb* this)
{
    return cpu_pcb_get_registros(this)->registroEBx;
}

uint32_t cpu_pcb_get_registro_ecx(t_cpu_pcb* this)
{
    return cpu_pcb_get_registros(this)->registroECx;
}

uint32_t cpu_pcb_get_registro_edx(t_cpu_pcb* this)
{
    return cpu_pcb_get_registros(this)->registroEDx;
}

void cpu_pcb_set_registro_eax(t_cpu_pcb* this, uint32_t registro)
{
    cpu_pcb_get_registros(this)->registroEAx = registro;
}

void cpu_pcb_set_registro_ebx(t_cpu_pcb* this, uint32_t registro)
{
    cpu_pcb_get_registros(this)->registroEBx = registro;
}

void cpu_pcb_set_registro_ecx(t_cpu_pcb* this, uint32_t registro)
{
    cpu_pcb_get_registros(this)->registroECx = registro;
}

void cpu_pcb_set_registro_edx(t_cpu_pcb* this, uint32_t registro)
{
    cpu_pcb_get_registros(this)->registroEDx = registro;
}

////////////////////////////  Registros 16bits  /////////////////////////////////////////

uint32_t cpu_pcb_get_registro_rax(t_cpu_pcb* this)
{
    return cpu_pcb_get_registros(this)->registroRAx;
}

uint32_t cpu_pcb_get_registro_rbx(t_cpu_pcb* this)
{
    return cpu_pcb_get_registros(this)->registroRBx;
}

uint32_t cpu_pcb_get_registro_rcx(t_cpu_pcb* this)
{
    return cpu_pcb_get_registros(this)->registroRCx;
}

uint32_t cpu_pcb_get_registro_rdx(t_cpu_pcb* this)
{
    return cpu_pcb_get_registros(this)->registroRDx;
}

void cpu_pcb_set_registro_rax(t_cpu_pcb* this, uint32_t registro)
{
    cpu_pcb_get_registros(this)->registroRAx = registro;
}

void cpu_pcb_set_registro_rbx(t_cpu_pcb* this, uint32_t registro)
{
    cpu_pcb_get_registros(this)->registroRBx = registro;
}

void cpu_pcb_set_registro_rcx(t_cpu_pcb* this, uint32_t registro)
{
    cpu_pcb_get_registros(this)->registroRCx = registro;
}

void cpu_pcb_set_registro_rdx(t_cpu_pcb* this, uint32_t registro)
{
    cpu_pcb_get_registros(this)->registroRDx = registro;
}