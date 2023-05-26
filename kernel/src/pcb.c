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

   return this;
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