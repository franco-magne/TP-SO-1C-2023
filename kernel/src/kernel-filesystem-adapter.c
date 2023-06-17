#include <../include/kernel-filesystem-adapter.h>

extern t_list* tablaGlobalDeArchivosAbiertos;
pthread_mutex_t mutexTablaGlobal = PTHREAD_MUTEX_INITIALIZER;


bool instruccion_f_open(t_pcb* pcb, t_kernel_config* kernelConfig, t_log* kernelLogger){

log_info(kernelLogger,"PID: <%i> - Abrir Archivo: <NOMBRE ARCHIVO>", pcb_get_pid(pcb));

char* nombreArchivo;
int index = 0;

index = list_get_index(tablaGlobalDeArchivosAbiertos,el_archivo_ya_existe,nombreArchivo);

if( index =! 0 ){

  t_kernel_archivo* unArchivo = list_get(tablaGlobalDeArchivosAbiertos, index);
  // kernel_archivo_aniadir_cola_procesos_bloqueados(unArchivo,pcb);

  t_pcb_archivo* unArchivoNuevo = archivo_create_pcb(nombreArchivo);
  archivo_pcb_set_victima(unArchivoNuevo, true);
  pcb_add_lista_de_archivos(pcb,unArchivoNuevo);
  
  return true;
} 

t_buffer* bufferNombreArchivo = buffer_create();
buffer_pack_string(bufferNombreArchivo, nombreArchivo);
stream_send_buffer(kernel_config_get_socket_fileSystem(kernelConfig), HEADER_f_open, bufferNombreArchivo);

uint8_t fileSystemResponse = stream_recv_header(kernel_config_get_socket_fileSystem(kernelConfig));
stream_recv_empty_buffer(kernel_config_get_socket_fileSystem(kernelConfig));

if(fileSystemResponse == HANDSHAKE_ok_continue){
    t_kernel_archivo* nuevoArchivo = archivo_create_kernel(pcb_get_pid(pcb), nombreArchivo);

    pthread_mutex_lock(&mutexTablaGlobal);
    list_add(tablaGlobalDeArchivosAbiertos,pcb);
    pthread_mutex_unlock(&mutexTablaGlobal);

    t_pcb_archivo* unArchivoNuevo = archivo_create_pcb(nombreArchivo);
    pcb_add_lista_de_archivos(pcb,unArchivoNuevo);

    return false;
}



//CHEQUEAR QUE ESE ARCHIVO NO ESTE EN LA LISTA DE TABLA GLOBAL
// SI NO ESTA AHI HABLO CON FILE SYSTEM

// TABLA GLOBAL DE ARCHIVOS T_LIST*
// {
// NOMBRE DE ARCHIVO
// PID DE ARCHIVO QUE ESTA UTILIZANDO EL ARCHIVO
// COLA DE PROCESOS QUE QUIEREN UTILIZAR EL ARCHIVO
// } 


// DENTRO DEL PCB VOY MI TABLA DE ARCHIVOS POR PROCESO T_LIST*
//{
//  
// NOMBRE 
//}

// LO QUE TE TENGO QUE MANDAR ES EL NOMBRE DEL ARCHIVO

// RESPONDER ?? 

}

void instruccion_f_close(t_pcb* pcbAIniciar, t_kernel_config* kernelConfig, t_log* kernelLogger){

log_info(kernelLogger,"PID: <%i> - Cerrar Archivo: <NOMBRE ARCHIVO>", pcb_get_pid(pcbAIniciar));



}

void instruccion_f_truncate(t_pcb* pcbAIniciar, t_kernel_config* kernelConfig, t_log* kernelLogger){

log_info(kernelLogger,"PID: <%i> - Archivo: <NOMBRE ARCHIVO> - Tamaño: <TAMAÑO>", pcb_get_pid(pcbAIniciar));


}

void instruccion_f_seek(t_pcb* pcbAIniciar, t_kernel_config* kernelConfig, t_log* kernelLogger){
    
log_info(kernelLogger,"PID: <%i> - Actualizar puntero Archivo: <NOMBRE ARCHIVO> - Puntero <PUNTERO>", pcb_get_pid(pcbAIniciar));


}