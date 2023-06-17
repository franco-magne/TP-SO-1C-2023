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
    this->victimaDelArchivo = true;

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



char* archivo_pcb_get_nombre_archivo(t_pcb_archivo* this){
    return this->nombreDeArchivo;
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


void archivo_pcb_set_victima(t_pcb_archivo* this, bool victimaArchivo)
{
     this->victimaDelArchivo = victimaArchivo;
}


///////////////////////////// FUNCIONES EXTRAS ///////////////////////////////


bool es_el_archivo_victima(t_pcb_archivo * unArchivo){
    return unArchivo->victimaDelArchivo;
}


bool el_archivo_ya_existe(t_kernel_archivo* unArchivo, t_kernel_archivo* otroArchivo)
{
    return (strcmp(unArchivo->nombreDeArchivo, otroArchivo->nombreDeArchivo) == 0);
}

bool elArchivoEsVictima(t_pcb_archivo* unArchivo, t_pcb_archivo* otroArchivo){

     return unArchivo->victimaDelArchivo;
}

char* archivo_motivo_de_bloqueo(t_list* listaDeArchivosDePcb)
{   
    t_pcb_archivo* aux = archivo_create_pcb(NULL);
    int index = list_get_index(listaDeArchivosDePcb,elArchivoEsVictima,aux);
    t_pcb_archivo* aux2= list_get(listaDeArchivosDePcb,index);
    
    free(aux);
    return aux2->nombreDeArchivo;
}

void modificar_victima_archivo(t_list* listaDeArchivosDePcb, bool cambio){
     
    t_pcb_archivo* aux = archivo_create_pcb(NULL);
    int index = list_get_index(listaDeArchivosDePcb,elArchivoEsVictima,aux);
    t_pcb_archivo* aux2= list_get(listaDeArchivosDePcb,index);
    aux2->victimaDelArchivo = cambio;
    list_replace(listaDeArchivosDePcb,index,aux2);
}