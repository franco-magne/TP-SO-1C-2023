#include <../include/kernel-estructura-archivos.h>

t_kernel_archivo* archivo_create_kernel(uint32_t pid, char* nombreArchivo){
    t_kernel_archivo* this = malloc(sizeof(*this));
    this->nombreDeArchivo = nombreArchivo;
    this->pidProceso = pid;
    this->colaDeProcesosEsperandoPorElArchivo = queue_create();

    return this;
}

t_pcb_archivo* archivo_create_pcb(char* nombreArchivo){
    t_pcb_archivo* this = malloc(sizeof(*this));
    this->nombreDeArchivo = nombreArchivo;
    this->victimaDelArchivo = false;

    return this;
}

///////////////////////// GETTER /////////////////////

char* kernel_archivo_get_nombre_archivo(t_kernel_archivo* this)
{
    return this->nombreDeArchivo;
}

uint32_t kernel_archivo_get_pid(t_kernel_archivo* this)
{
    return this->pidProceso;
}

t_queue* kernel_archivo_get_cola_procesos_bloqueados(t_kernel_archivo* this)
{
    return this->colaDeProcesosEsperandoPorElArchivo;
}



bool archivo_pcb_get_victima(t_pcb_archivo* this)
{
    return this->victimaDelArchivo;
}

///////////////////////// SETTER /////////////////////////////

void kernel_archivo_set_pid(t_kernel_archivo* this , uint32_t pid)
{
    this->pidProceso = pid;
}

/*void kernel_archivo_aniadir_cola_procesos_bloqueados(t_kernel_archivo* this, t_pcb* unPcb)
{
    queue_push(this->colaDeProcesosEsperandoPorElArchivo,unPcb);
}*/

void archivo_pcb_set_victima(t_pcb_archivo* this, bool victimaArchivo)
{
     this->victimaDelArchivo = victimaArchivo;
}


///////////////////////////// FUNCIONES EXTRAS ///////////////////////////////
/*
t_pcb* kernel_archivo_quitar_cola_procesos_bloqueados(t_kernel_archivo* this)
{
    return queue_pop(this->colaDeProcesosEsperandoPorElArchivo);
}
*/
bool el_archivo_ya_existe(char* unArchivo, char* otroArchivo)
{
    return (strcmp(unArchivo, otroArchivo) == 0);
}

bool elArchivoEsVictima(t_pcb_archivo* unArchivo, t_pcb_archivo* otroArchivo){

    return unArchivo->victimaDelArchivo || otroArchivo->victimaDelArchivo;
}

char* archivo_motivo_de_bloqueo(t_list* listaDeArchivosDePcb)
{   
    t_pcb_archivo* aux = archivo_create_pcb(NULL);
    int index = list_get_index(listaDeArchivosDePcb,elArchivoEsVictima,aux);
    t_pcb_archivo* aux2= list_get(listaDeArchivosDePcb,index);
    
    free(aux);
    return aux2->nombreDeArchivo;
}