#include <../include/kernel-filesystem-adapter.h>

extern t_list* tablaGlobalDeArchivosAbiertos;
pthread_mutex_t mutexTablaGlobal = PTHREAD_MUTEX_INITIALIZER;



bool file_system_adapter_chequear_si_ya_existe(t_pcb* pcb, t_kernel_config* kernelConfig, t_log* kernelLogger) {

  t_pcb_archivo* archivoAbrir = list_find(pcb_get_lista_de_archivos_abiertos(pcb), es_el_archivo_victima);

  char* nombreArchivo = archivo_pcb_get_nombre_archivo(archivoAbrir);

  log_info(kernelLogger,BOLD UNDERLINE CYAN"PID: <%i> - Abrir Archivo:"RESET BOLD YELLOW" <%s>", pcb_get_pid(pcb), nombreArchivo);

  int index = 0;
  
  t_kernel_archivo* unArchivo = archivo_create_kernel(-1,nombreArchivo );


  index = list_get_index(tablaGlobalDeArchivosAbiertos,el_archivo_ya_existe,unArchivo);

  if( index != -1 ){

      modificar_victima_archivo(pcb_get_lista_de_archivos_abiertos(pcb),nombreArchivo, false);
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
  char* nombreArchivo = archivo_pcb_get_nombre_archivo(archivoAbrir);
  t_buffer* bufferNombreArchivo = buffer_create();
  buffer_pack_string(bufferNombreArchivo, nombreArchivo);
  stream_send_buffer(kernel_config_get_socket_file_system(kernelConfig), HEADER_f_open, bufferNombreArchivo);


}

void file_system_adapter_recv_f_open(t_pcb* pcb, t_kernel_config* kernelConfig){ 

  t_pcb_archivo* archivoAbrir = list_find(pcb_get_lista_de_archivos_abiertos(pcb), es_el_archivo_victima);
  char* nombreArchivo = archivo_pcb_get_nombre_archivo(archivoAbrir);
  modificar_victima_archivo(pcb_get_lista_de_archivos_abiertos(pcb),nombreArchivo, false);


  uint8_t fileSystemResponse = stream_recv_header(kernel_config_get_socket_file_system(kernelConfig));
  stream_recv_empty_buffer(kernel_config_get_socket_file_system(kernelConfig));

  
  if(fileSystemResponse == HEADER_f_create){
  t_buffer* bufferCreate = buffer_create();
  buffer_pack_string(bufferCreate,nombreArchivo);
  stream_send_buffer(kernel_config_get_socket_file_system(kernelConfig), HEADER_f_create,bufferCreate);
  buffer_destroy(bufferCreate);
  fileSystemResponse = stream_recv_header(kernel_config_get_socket_file_system(kernelConfig));
  stream_recv_empty_buffer(kernel_config_get_socket_file_system(kernelConfig));
  }

  if(fileSystemResponse == HANDSHAKE_ok_continue){
    t_kernel_archivo* nuevoArchivo = archivo_create_kernel(pcb_get_pid(pcb),nombreArchivo);
    pthread_mutex_lock(&mutexTablaGlobal);
    list_add(tablaGlobalDeArchivosAbiertos,nuevoArchivo);
    pthread_mutex_unlock(&mutexTablaGlobal);

  }

}


t_pcb* atender_f_close(char* nombreArchivo, t_log* kernelLogger){

  pthread_mutex_lock(&mutexTablaGlobal);
  int index = archivo_kernel_index(tablaGlobalDeArchivosAbiertos,nombreArchivo);
  t_kernel_archivo* archivo = list_get(tablaGlobalDeArchivosAbiertos, index);
  pthread_mutex_unlock(&mutexTablaGlobal);


  if( queue_is_empty (kernel_archivo_get_cola_procesos_bloqueados(archivo)) ){
        pthread_mutex_lock(&mutexTablaGlobal);
        list_remove(tablaGlobalDeArchivosAbiertos,index);
        log_info(kernelLogger, GREEN BOLD  "ARCHIVO "RESET YELLOW BOLD  " <%s> "RESET GREEN BOLD " ELIMINADO DE LA TABLA GLOBAL ",nombreArchivo);
        pthread_mutex_unlock(&mutexTablaGlobal);
        return NULL;
  } 
  else {
      t_pcb* pcbLiberado = queue_pop( kernel_archivo_get_cola_procesos_bloqueados(archivo) );
      pthread_mutex_lock(&mutexTablaGlobal);
      kernel_archivo_set_cola_procesos_bloqueados(archivo, kernel_archivo_get_cola_procesos_bloqueados(archivo) );
      list_replace(tablaGlobalDeArchivosAbiertos,index,archivo);
      pthread_mutex_unlock(&mutexTablaGlobal);

      return  pcbLiberado;
  }
}


void file_system_adapter_send_f_truncate(t_pcb* pcb, t_kernel_config* kernelConfig, t_log* kernelLogger){ 

  t_pcb_archivo* archivoTruncar = list_find(pcb_get_lista_de_archivos_abiertos(pcb), es_el_archivo_victima);

  char* nombreArchivo = archivo_pcb_get_nombre_archivo(archivoTruncar);
  uint32_t tamanioArchivo = archivo_pcb_get_tamanio_archivo(archivoTruncar);

  log_info(kernelLogger,BOLD UNDERLINE CYAN"PID: <%i> - Archivo: "RESET BOLD YELLOW"<%s> "RESET BOLD UNDERLINE CYAN" - Tamaño: <%i>", pcb_get_pid(pcb),nombreArchivo,tamanioArchivo);

  t_buffer* bufferTruncate = buffer_create();

  buffer_pack_string(bufferTruncate,nombreArchivo );
  buffer_pack(bufferTruncate, &tamanioArchivo, sizeof(tamanioArchivo));

  stream_send_buffer(kernel_config_get_socket_file_system(kernelConfig), HEADER_f_truncate, bufferTruncate);

  int index = index_de_archivo_pcb(pcb_get_lista_de_archivos_abiertos(pcb),nombreArchivo);
  archivoTruncar = list_get(pcb_get_lista_de_archivos_abiertos(pcb), index);
  archivo_pcb_set_victima(archivoTruncar,false);
  list_replace(pcb_get_lista_de_archivos_abiertos(pcb),index,archivoTruncar);

  buffer_destroy(bufferTruncate);

}


void file_system_adapter_recv_f_truncate(t_kernel_config* kernelConfig, t_log* kernelLogger){
    
    uint8_t fileSystemResponse = stream_recv_header(kernel_config_get_socket_file_system(kernelConfig));
    stream_recv_empty_buffer(kernel_config_get_socket_file_system(kernelConfig));

    if(fileSystemResponse == HANDSHAKE_ok_continue){
      log_info(kernelLogger,BLUE ITALIC "EL ARCHIVO FUE MODIFICADO");
    }
}




void atender_f_seek(t_pcb* pcb, t_kernel_config* kernelConfig, t_log* kernelLogger){
    
  t_pcb_archivo* archivoFSeek = list_find(pcb_get_lista_de_archivos_abiertos(pcb), es_el_archivo_victima);

  char* nombreArchivo = archivo_pcb_get_nombre_archivo(archivoFSeek);
  uint32_t punteroArchivo = archivo_pcb_get_puntero_archivo(archivoFSeek);

  log_info(kernelLogger,BOLD UNDERLINE CYAN "PID: <%i> - Actualizar puntero Archivo: "RESET BOLD YELLOW" <%s> "RESET BOLD UNDERLINE CYAN" - Puntero <%i>", pcb_get_pid(pcb),nombreArchivo,punteroArchivo);

  int index = index_de_archivo_pcb(pcb_get_lista_de_archivos_abiertos(pcb),nombreArchivo);
  archivoFSeek = list_get(pcb_get_lista_de_archivos_abiertos(pcb), index);
  archivo_pcb_set_victima(archivoFSeek,false);
  list_replace(pcb_get_lista_de_archivos_abiertos(pcb),index,archivoFSeek);


}

void file_system_adapter_send_f_read(t_pcb* pcb, t_log* kernelLogger, t_kernel_config* kernelConfig){
    uint32_t pid = pcb_get_pid(pcb);
    t_pcb_archivo* archivoLeer = list_find(pcb_get_lista_de_archivos_abiertos(pcb), es_el_archivo_victima);
    char* nombreArchivo = archivo_pcb_get_nombre_archivo(archivoLeer);
    uint32_t puntero = archivo_pcb_get_puntero_archivo(archivoLeer);
    uint32_t tamanioArchivo = archivo_pcb_get_tamanio_archivo(archivoLeer);
    uint32_t direccionFisica = archivo_pcb_get_direccion_fisica(archivoLeer);
    uint32_t cantidadByte = archivo_pcb_get_cantidad_byte(archivoLeer);
    uint32_t desplazamientoFisico = archivo_pcb_get_desplazamiento_fisico(archivoLeer);
    log_info(kernelLogger,BOLD UNDERLINE CYAN "PID: <%i> - Leer Archivo: "RESET BOLD YELLOW"<%s> "RESET BOLD UNDERLINE CYAN" - Puntero <%i> - Dirección Memoria <%i> - Tamaño <%i>",pid,nombreArchivo,puntero,direccionFisica,tamanioArchivo);

     t_buffer* bufferFRead = buffer_create();

    buffer_pack_string(bufferFRead,nombreArchivo);
    buffer_pack(bufferFRead, &direccionFisica, sizeof(direccionFisica));
    buffer_pack(bufferFRead, &cantidadByte, sizeof(cantidadByte));
    buffer_pack(bufferFRead, &puntero, sizeof(puntero));
    buffer_pack(bufferFRead,&desplazamientoFisico, sizeof(desplazamientoFisico));

    
    stream_send_buffer(kernel_config_get_socket_file_system(kernelConfig),HEADER_f_read,bufferFRead);

    buffer_destroy(bufferFRead);

    



}

void file_system_adapter_recv_f_read(t_kernel_config* kernelConfig, t_log* kernelLogger){

  uint8_t fileSystemResponse = stream_recv_header(kernel_config_get_socket_file_system(kernelConfig));
    stream_recv_empty_buffer(kernel_config_get_socket_file_system(kernelConfig));
  if(fileSystemResponse == HANDSHAKE_ok_continue){

      log_info(kernelLogger,BLUE ITALIC "EL ARCHIVO FUE LEIDO");
  }

}



void file_system_adapter_send_f_write(t_pcb* pcb, t_log* kernelLogger, t_kernel_config* kernelConfig){

   uint32_t pid = pcb_get_pid(pcb);
    t_pcb_archivo* archivoEscribir = list_find(pcb_get_lista_de_archivos_abiertos(pcb), es_el_archivo_victima);
    char* nombreArchivo = archivo_pcb_get_nombre_archivo(archivoEscribir);
    uint32_t cantidadByte = archivo_pcb_get_cantidad_byte(archivoEscribir);
    uint32_t puntero = archivo_pcb_get_puntero_archivo(archivoEscribir);
    uint32_t tamanioArchivo = archivo_pcb_get_tamanio_archivo(archivoEscribir);
    uint32_t direccionFisica = archivo_pcb_get_direccion_fisica(archivoEscribir);
    uint32_t desplazamientoFisico = archivo_pcb_get_desplazamiento_fisico(archivoEscribir);

  
    log_info(kernelLogger,BOLD UNDERLINE CYAN "PID: <%i> - Escribir Archivo: "RESET BOLD YELLOW"<%s> "RESET BOLD UNDERLINE CYAN"- Puntero <%i> - Dirección Memoria <%i> - Tamaño <%i>",pid,nombreArchivo,puntero,direccionFisica,tamanioArchivo);

    t_buffer* bufferFWrite = buffer_create();

    buffer_pack_string(bufferFWrite,nombreArchivo);
    buffer_pack(bufferFWrite, &direccionFisica, sizeof(direccionFisica));
    buffer_pack(bufferFWrite, &cantidadByte, sizeof(cantidadByte));
    buffer_pack(bufferFWrite, &puntero, sizeof(puntero));
    buffer_pack(bufferFWrite, &desplazamientoFisico, sizeof(desplazamientoFisico));

    
    stream_send_buffer(kernel_config_get_socket_file_system(kernelConfig),HEADER_f_write,bufferFWrite);

    buffer_destroy(bufferFWrite);



}


void file_system_adapter_recv_f_write(t_kernel_config* kernelConfig, t_log* kernelLogger){

  uint8_t fileSystemResponse = stream_recv_header(kernel_config_get_socket_file_system(kernelConfig));
    stream_recv_empty_buffer(kernel_config_get_socket_file_system(kernelConfig));
  if(fileSystemResponse == HANDSHAKE_ok_continue){
    
      log_info(kernelLogger,BLUE ITALIC "EL ARCHIVO FUE ESCRITO");
  }

}
