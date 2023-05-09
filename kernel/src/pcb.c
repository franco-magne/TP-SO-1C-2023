#include "../include/pcb.h"



t_pcb* pcb_create(uint32_t pid) 
{
   printf("Creando nuevo PCB con PID %d\n", pid); 
    t_pcb* self = malloc(sizeof(t_pcb*));
   if (self == NULL) {
      printf("Error al asignar memoria para el PCB\n");
      exit(EXIT_FAILURE);
   }
   self->pid = pid;
   self->estadoActual= NEW;
   self->programCounter=0;
   self->instruccionesBuffer = NULL;
   self->registros = REGISTRO_null;

   return self;
}
//////////////////////// GETTERS /////////////////////

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

void pcb_set_estado_actual(t_pcb* this, uint32_t pid) 
{
   this->pid = pid;
}
