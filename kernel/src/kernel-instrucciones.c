#include <../include/kernel-instrucciones.h>

extern t_log* kernelLogger;
extern t_kernel_config* kernelConfig;


extern t_kernel_recurso* recursoConfig;
extern char* nombre_recurso;
extern pthread_mutex_t mutexCantidadRecursos;
extern pthread_mutex_t mutexNombreRecurso;

extern t_estado* estadoNew;
extern t_estado* estadoReady;
extern t_estado* estadoExec;
extern t_estado* estadoBlocked;
extern t_estado* estadoExit;




static void pid_destroyer(void* pidADestruir)
{
    free(pidADestruir);
}

static void* pcb_to_pid_transformer(void* pcbATransformar)
{
    t_pcb* tempPcbATransformar = (t_pcb*) pcbATransformar;
    uint32_t* tempPid = malloc(sizeof(*tempPid));
    
    *tempPid = tempPcbATransformar->pid;

    return (void*) tempPid; 
}



char* string_pids_ready(t_estado* estadoReady)
{
    t_list* tempPidList;
    char* listaPidsString = string_new();
    uint32_t tempPid;

    pthread_mutex_lock(estado_get_mutex(estadoReady));
    tempPidList = list_map(estadoReady->listaProcesos, pcb_to_pid_transformer);
    pthread_mutex_unlock(estado_get_mutex(estadoReady));

    string_append(&listaPidsString, "[");

    for(int i = 0; i < tempPidList->elements_count; i++) {
        
        tempPid = *(uint32_t*)list_get(tempPidList, i);

        char* stringPid = string_itoa(tempPid);
        string_append(&listaPidsString, stringPid);
        free(stringPid);
        
        if(i != tempPidList->elements_count - 1) {

            string_append(&listaPidsString, ", ");
        }
    }

    string_append(&listaPidsString, "]");

    list_destroy_and_destroy_elements(tempPidList, pid_destroyer);
    return listaPidsString;
}


void proceso_pasa_a_ready(t_pcb* pcb, char* estadoActual){
    pcb_set_estado_anterior(pcb, pcb_get_estado_actual(pcb));
    pcb_set_estado_actual(pcb, READY);
    //setear_tiempo_ready(pcb); // EMPIEZA A CONTAR EL TIEMPO 
    pcb->tiempo_ready = temporal_create();
    estado_encolar_pcb_atomic(estadoReady, pcb);
    char* stringPidsReady = string_pids_ready(estadoReady);
    log_transition(estadoActual, "READY", pcb_get_pid(pcb));
    log_info(kernelLogger, BOLD MAGENTA UNDERLINE "Cola Ready <%s>: %s" RESET, kernel_config_get_algoritmo_planificacion(kernelConfig), stringPidsReady);
    free(stringPidsReady);
    sem_post(estado_get_sem(estadoReady));
}

void proceso_pasa_a_bloqueado(t_pcb* pcb, char* motivoDelBloqueo){
      log_transition("EXEC", "BLOCK", pcb_get_pid(pcb));
      log_info(kernelLogger, BOLD GREEN "PID: <%i> - Bloqueado por:"RESET BOLD UNDERLINE"<%s>", pcb_get_pid(pcb), motivoDelBloqueo);

      pcb_set_estado_anterior(pcb, pcb_get_estado_actual(pcb));
      pcb_set_estado_actual(pcb, BLOCK);
      estado_encolar_pcb_atomic(estadoBlocked, pcb);
}

void proceso_pasa_a_exit(t_pcb* pcbAExit){
    pcb_set_estado_actual(pcbAExit, EXIT);
    estado_encolar_pcb_atomic(estadoExit, pcbAExit);
    log_transition("EXEC", "EXIT", pcb_get_pid(pcbAExit));
    //stream_send_empty_buffer(pcb_get_socket(pcb), HEADER_proceso_terminado);
    sem_post(estado_get_sem(estadoExit));
    //pcb_set_proceso_bloqueado_o_terminado_atomic(pcb, true);
    //terminar_proceso(pcb);
}

////////////////////////////////////////// INSTRUCCION EXIT ////////////////////////////////////
void instruccion_exit(t_pcb* pcb){
    
    proceso_pasa_a_exit(pcb);
            
}

////////////////////////////////////// INSTRUCCION IO /////////////////////////////////////////
void instruccion_io(t_pcb* pcb) 
{
    
    proceso_pasa_a_bloqueado(pcb, "IO");


    
    log_info(kernelLogger,BOLD UNDERLINE CYAN "PID: <%d> - Ejecuta IO: <%is>" RESET, pcb_get_pid(pcb), pcb_get_tiempoIO(pcb));
    sleep(pcb_get_tiempoIO(pcb));
    int cantidadPcbsEnLista = list_size(estado_get_list(estadoBlocked));
    
    if (cantidadPcbsEnLista == 1) {
        pcb = estado_desencolar_primer_pcb(estadoBlocked);
    } 
    else {
        pcb = estado_remover_pcb_de_cola_atomic(estadoBlocked,pcb);
    }
    
    proceso_pasa_a_ready(pcb, "BLOCK");

    
    
}

///////////////////////////////////////////// INSTRUCCION YIELD ///////////////////////////////////////

void instruccion_yield(t_pcb* pcb){

    proceso_pasa_a_ready(pcb, "EXEC");                
}

///////////////////////////////////////////// INSTRUCCION WAIT ///////////////////////////////////////

bool recurso_disponible(int posicion_recurso){
    return ( *(recursoConfig[posicion_recurso].instancias_recurso) > 0);
}


void asignar_recurso(int posicion_recurso){
    pthread_mutex_lock(&mutexCantidadRecursos);
    *(recursoConfig[posicion_recurso].instancias_recurso) -= 1;
    pthread_mutex_unlock(&mutexCantidadRecursos);

}

bool instruccion_wait(t_pcb* pcb) {
    char* recursoUtilizado = pcb_get_recurso_utilizado(pcb);
    
    if (contains(kernel_config_get_recurso(kernelConfig), recursoUtilizado)) {
        int posicion_recurso = position_in_list(kernel_config_get_recurso(kernelConfig), recursoUtilizado);

        if (recurso_disponible(posicion_recurso)) {
            asignar_recurso(posicion_recurso);
            log_info(kernelLogger,BOLD UNDERLINE CYAN "PID: <%i> - Wait:"RESET BOLD GREEN" <%s> "RESET BOLD UNDERLINE CYAN"- Instancias: <%i> ",pcb_get_pid(pcb),  recursoConfig[posicion_recurso].recurso , *recursoConfig[posicion_recurso].instancias_recurso );    
        } else {
            asignar_recurso(posicion_recurso);
            log_info(kernelLogger, "RECURSO QUE SE BLOQUEA <%s> PCB <%i> ",recursoUtilizado,pcb->pid );
            pcb_set_recurso_utilizado(pcb,recursoUtilizado);
            proceso_pasa_a_bloqueado(pcb,recursoConfig[posicion_recurso].recurso);
            return true;
        }
    } else {
        log_error(kernelLogger, BACKGROUND_RED BOLD GREEN "RECURSO NO EXISTE POR EL PROCESO QUE PIDE " RESET);    
        proceso_pasa_a_exit(pcb);
        return false;

    }

}

///////////////////////////////// INSTRUCCION SIGNAL //////////////////////////////

bool pcb_esta_bloqueado_por_recurso(void* pcb, void* pcb_aux){
return (strcmp(pcb_get_recurso_utilizado(pcb),pcb_get_recurso_utilizado(pcb_aux)) == 0);
}


void devolver_recurso(int posicion_recurso){

    pthread_mutex_lock(&mutexCantidadRecursos);
    *(recursoConfig[posicion_recurso].instancias_recurso) += 1;
    pthread_mutex_unlock(&mutexCantidadRecursos);

}

t_pcb* primer_elemento_bloqueado_por_recurso(char* nombreRecurso) {
    t_pcb* pcb;

    pthread_mutex_lock(&mutexNombreRecurso);
    t_pcb* pcb_aux = pcb_create(-1);
    pcb_aux->recursoUtilizado = strdup(nombreRecurso); // Utilizar strdup para crear una copia de la cadena
    log_info(kernelLogger, "RECURSO A LIBERAR "RED BOLD" <%s>", nombreRecurso);
    t_list* listAux = list_filter_ok(estado_get_list(estadoBlocked), pcb_esta_bloqueado_por_recurso, pcb_aux);
    pthread_mutex_unlock(&mutexNombreRecurso);

    int cantidadPcbsEnLista = list_size(estado_get_list(estadoBlocked));
    if (cantidadPcbsEnLista == 1) {
        pcb = estado_desencolar_primer_pcb(estadoBlocked);
        list_destroy(listAux);
        return pcb;
    } else if (cantidadPcbsEnLista > 1) {
        pthread_mutex_lock(&mutexNombreRecurso);
        pcb = list_get(listAux, 0);
        pthread_mutex_unlock(&mutexNombreRecurso);
        pcb = estado_remover_pcb_de_cola_atomic(estadoBlocked, pcb);
        list_destroy(listAux);
        return pcb;
    }

    list_destroy(listAux);
    return NULL;
}

void instruccion_signal(t_pcb* pcb){
    
        if(contains(kernel_config_get_recurso(kernelConfig) , pcb_get_recurso_utilizado(pcb)) ){
            
            int posicion_recurso = position_in_list(kernel_config_get_recurso(kernelConfig) , pcb_get_recurso_utilizado(pcb));
            devolver_recurso(posicion_recurso);
            log_info(kernelLogger,BOLD UNDERLINE CYAN "PID: <%i> - Signal: "RESET BOLD GREEN" <%s> "BOLD UNDERLINE CYAN" - Instancias: <%i> ",pcb_get_pid(pcb), recursoConfig[posicion_recurso].recurso , *recursoConfig[posicion_recurso].instancias_recurso );    
            t_pcb* pcbPasaReady = primer_elemento_bloqueado_por_recurso(pcb_get_recurso_utilizado(pcb));
            if(pcbPasaReady != NULL){
                proceso_pasa_a_ready(pcbPasaReady,"BLOCK");
            }
        } 
        else {
            log_error(kernelLogger, BACKGROUND_RED BOLD GREEN "RECURSO NO EXISTE POR EL PROCESO QUE LO DEVUELVE " RESET);    
            proceso_pasa_a_exit(pcb);
        }

}

///////////////////////////////// INSTRUCCION CREATE_SEGMENT //////////////////////////////

bool instruccion_create_segment(t_pcb* pcb){

    memoria_adapter_enviar_create_segment(pcb,kernelConfig);

    uint8_t memoriaResponse = memoria_adapter_recibir_create_segment(pcb, kernelConfig, kernelLogger);

    if(memoriaResponse == HEADER_memoria_insuficiente){
        liberar_segmentos_del_proceso_tabla_global(pcb);
        memoria_adapter_enviar_finalizar_proceso(pcb,kernelConfig,kernelLogger,BOLD BLUE "OUT_OF_MEMORY" RESET);
        proceso_pasa_a_exit(pcb);
        return true;
    }
    return false;
}

///////////////////////////////// INSTRUCCION DELETE_SEGMENT //////////////////////////////

void instruccion_delete_segment(t_pcb* pcb){

    memoria_adapter_enviar_delete_segment(pcb , kernelConfig);
    memoria_adapter_recibir_delete_segment(pcb, kernelConfig, kernelLogger);


}

///////////////////////////////// INSTRUCCION F_OPEN //////////////////////////////

bool instruccion_f_open(t_pcb* pcb){

    bool existeElArchivo = file_system_adapter_chequear_si_ya_existe(pcb,kernelConfig, kernelLogger);

    if(existeElArchivo){
        proceso_pasa_a_bloqueado(pcb, BOLD YELLOW "ARCHIVO");
        return true;
    }

    file_system_adapter_send_f_open(pcb,kernelConfig);
    file_system_adapter_recv_f_open(pcb,kernelConfig);

    return false;


}


///////////////////////////// F_CLOSE /////////////////////////////////

void instruccion_f_close(t_pcb* pcb){

    t_pcb_archivo* archivoAbrir = list_find(pcb_get_lista_de_archivos_abiertos(pcb), es_el_archivo_victima);
    char* nombreArchivo = archivo_pcb_get_nombre_archivo(archivoAbrir);

    log_info(kernelLogger,BOLD UNDERLINE CYAN "PID: <%i> - Cerrar Archivo: "RESET BOLD YELLOW"<%s>", pcb_get_pid(pcb), nombreArchivo);

    t_pcb* pcbBloqueado = atender_f_close(nombreArchivo, kernelLogger);

    eliminar_archivo_pcb(pcb_get_lista_de_archivos_abiertos(pcb),nombreArchivo);

    if(pcbBloqueado == NULL){
    
    }
    else{

    pcbBloqueado = estado_remover_pcb_de_cola_atomic(estadoBlocked,pcbBloqueado);
    proceso_pasa_a_ready(pcbBloqueado, "BLOCK");

    }


}

/////////////////////////////// F_TRUNCATE ////////////////////////////////////

void instruccion_f_truncate(t_pcb* pcb){

file_system_adapter_send_f_truncate(pcb, kernelConfig, kernelLogger);
proceso_pasa_a_bloqueado(pcb, BOLD YELLOW "ARCHIVO");
file_system_adapter_recv_f_truncate(kernelConfig, kernelLogger);
pcb = estado_remover_pcb_de_cola_atomic(estadoBlocked,pcb);
proceso_pasa_a_ready(pcb, "BLOCK");


}

/////////////////////// F_SEEK ///////////////////////

void instruccion_f_seek(t_pcb* pcb){

    atender_f_seek(pcb,kernelConfig, kernelLogger);

}


////////////////////// F_WRITE //////////////////////

void instruccion_f_write(t_pcb* pcb){
   
    t_pcb_archivo* archivoEscribir = list_find(pcb_get_lista_de_archivos_abiertos(pcb), es_el_archivo_victima);
    char* nombreArchivo = archivo_pcb_get_nombre_archivo(archivoEscribir);
    file_system_adapter_send_f_write(pcb, kernelLogger,kernelConfig);
    proceso_pasa_a_bloqueado(pcb, BOLD YELLOW "ARCHIVO F_WRITE");
    file_system_adapter_recv_f_write(kernelConfig, kernelLogger);
    pcb = estado_remover_pcb_de_cola_atomic(estadoBlocked,pcb);
    proceso_pasa_a_ready(pcb, "BLOCK");


    int index = index_de_archivo_pcb(pcb_get_lista_de_archivos_abiertos(pcb),nombreArchivo);
    archivo_pcb_set_victima(archivoEscribir,false);
    list_replace(pcb_get_lista_de_archivos_abiertos(pcb), index ,archivoEscribir);

}

////////////////////// F_READ ///////////////////////

void instruccion_f_read(t_pcb* pcb){
    uint32_t pid = pcb_get_pid(pcb);
    t_pcb_archivo* archivoLeer = list_find(pcb_get_lista_de_archivos_abiertos(pcb), es_el_archivo_victima);
    char* nombreArchivo = archivo_pcb_get_nombre_archivo(archivoLeer);

    file_system_adapter_send_f_read(pcb,kernelLogger,kernelConfig);
    proceso_pasa_a_bloqueado(pcb, BOLD YELLOW "ARCHIVO F_READ");
    file_system_adapter_recv_f_read(kernelConfig, kernelLogger);
    pcb = estado_remover_pcb_de_cola_atomic(estadoBlocked,pcb);
    proceso_pasa_a_ready(pcb, "BLOCK");

    
    int index = index_de_archivo_pcb(pcb_get_lista_de_archivos_abiertos(pcb),nombreArchivo);
    archivo_pcb_set_victima(archivoLeer,false);
    list_replace(pcb_get_lista_de_archivos_abiertos(pcb), index ,archivoLeer);

}