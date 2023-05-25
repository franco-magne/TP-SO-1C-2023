#include <../include/cpu-pcb.h>

t_cpu_pcb* cpu_pcb_create (uint32_t pid, uint32_t programCounter, t_registros_cpu* registrosCpu)
{
    t_cpu_pcb* this = malloc(sizeof(*this));
    this->pid = pid;
    this->programCounter = programCounter;
    this->registrosCpu = registrosCpu;
    //this->arrayDeSegmentos = arrayDeSegmentos;
    //this->estimadoProxRafaga = estimadoProxRafaga;
    //this->tllegadaReady = tllegadaReady;
    this->instrucciones = NULL;
    
    return this;
}

void cpu_pcb_destroy(t_cpu_pcb* this) 
{
    free(this->arrayDeSegmentos);
   // destroy_instructions_list(this->instrucciones);
    free(this->registrosCpu);

    free(this);
}

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

/*uint32_t* cpu_pcb_get_array_segmentos(t_cpu_pcb* this) 
{
    return this->arrayDeSegmentos;
}*/

/*void cpu_pcb_set_array_segmentos(t_cpu_pcb* this, uint32_t* arrayDeSegmentos) 
{
    this->arrayDeSegmentos = arrayDeSegmentos;
}*/

t_registros_cpu* cpu_pcb_get_registros(t_cpu_pcb* this)
{
    return this->registrosCpu;
}

void cpu_pcb_set_registros(t_cpu_pcb* this, t_registros_cpu* registrosCpu)
{
    this->registrosCpu = registrosCpu;
}

////////////////////////////  Registros 4bits  /////////////////////////////////////////
uint32_t cpu_pcb_get_registro_ax(t_cpu_pcb* this)
{
    return cpu_pcb_get_registros(this)->registroAx;

}

uint32_t cpu_pcb_get_registro_bx(t_cpu_pcb* this)
{
    return cpu_pcb_get_registros(this)->registroBx;
}

uint32_t cpu_pcb_get_registro_cx(t_cpu_pcb* this)
{
    return cpu_pcb_get_registros(this)->registroCx;
}

uint32_t cpu_pcb_get_registro_dx(t_cpu_pcb* this)
{
    return cpu_pcb_get_registros(this)->registroDx;
}

void cpu_pcb_set_registro_ax(t_cpu_pcb* this, uint32_t registro)
{
    cpu_pcb_get_registros(this)->registroAx = registro;
}

void cpu_pcb_set_registro_bx(t_cpu_pcb* this, uint32_t registro)
{
    cpu_pcb_get_registros(this)->registroBx = registro;
}

void cpu_pcb_set_registro_cx(t_cpu_pcb* this, uint32_t registro)
{
    cpu_pcb_get_registros(this)->registroCx = registro;
}

void cpu_pcb_set_registro_dx(t_cpu_pcb* this, uint32_t registro)
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