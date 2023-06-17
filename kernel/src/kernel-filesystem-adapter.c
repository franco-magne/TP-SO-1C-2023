#include <../include/kernel-filesystem-adapter.h>

extern t_list* tablaGlobalDeArchivosAbiertos;
pthread_mutex_t mutexTablaGlobal = PTHREAD_MUTEX_INITIALIZER;



bool file_system_adapter_chequear_si_ya_existe(t_pcb* pcb, t_kernel_config* kernelConfig, t_log* kernelLogger) {

  t_pcb_archivo* archivoAbrir = list_find(pcb_get_lista_de_archivos_abiertos(pcb), es_el_archivo_victima);

  char* nombreArchivo = archivo_pcb_get_nombre_archivo(pcb);

  log_info(kernelLogger,"PID: <%i> - Abrir Archivo: <%s>", pcb_get_pid(pcb), nombreArchivo);

  int index = 0;

  index = list_get_index(tablaGlobalDeArchivosAbiertos,el_archivo_ya_existe,nombreArchivo);

  if( index =! 0 ){

      modificar_victima_archivo(pcb_get_lista_de_archivos_abiertos(pcb), false);
      t_kernel_archivo* unArchivo = list_get(tablaGlobalDeArchivosAbiertos, index);
      queue_push(unArchivo->colaDeProcesosEsperandoPorElArchivo,pcb);
      pthread_mutex_lock(&mutexTablaGlobal);
      list_replace(tablaGlobalDeArchivosAbiertos,index,unArchivo);
      pthread_mutex_unlock(&mutexTablaGlobal);

      return true;
  } 

  return false;
}

void file_system_adapter_send_f_open(t_pcb* pcb, t_kernel_config* kernelConfig){ 
  
  t_pcb_archivo* archivoAbrir = list_find(pcb_get_lista_de_archivos_abiertos(pcb), es_el_archivo_victima);
  char* nombreArchivo = archivo_pcb_get_nombre_archivo(pcb);

  t_buffer* bufferNombreArchivo = buffer_create();
  buffer_pack_string(bufferNombreArchivo, nombreArchivo);
  stream_send_buffer(kernel_config_get_socket_file_system(kernelConfig), HEADER_f_open, bufferNombreArchivo);


}

void file_system_adapter_recv_f_open(t_pcb* pcb, t_kernel_config* kernelConfig){ 

  t_pcb_archivo* archivoAbrir = list_find(pcb_get_lista_de_archivos_abiertos(pcb), es_el_archivo_victima);
  char* nombreArchivo = archivo_pcb_get_nombre_archivo(pcb);
  modificar_victima_archivo(pcb_get_lista_de_archivos_abiertos(pcb), false);


  uint8_t fileSystemResponse = stream_recv_header(kernel_config_get_socket_file_system(kernelConfig));
  stream_recv_empty_buffer(kernel_config_get_socket_file_system(kernelConfig));

  if(fileSystemResponse == HANDSHAKE_ok_continue){
    t_kernel_archivo* nuevoArchivo = archivo_create_kernel(pcb_get_pid(pcb),nombreArchivo);
    pthread_mutex_lock(&mutexTablaGlobal);
    list_add(tablaGlobalDeArchivosAbiertos,nuevoArchivo);
    pthread_mutex_unlock(&mutexTablaGlobal);

    t_pcb_archivo* unArchivoNuevo = archivo_create_pcb(nombreArchivo);
    pcb_add_lista_de_archivos(pcb,unArchivoNuevo);

  }

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