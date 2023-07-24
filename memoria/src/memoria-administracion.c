#include <../include/memoria-administracion.h>

extern t_list* listaDeSegmentos;
extern void* memoriaPrincipal;
extern t_log *memoriaLogger;
extern t_memoria_config* memoriaConfig;
extern pthread_mutex_t mutexListaDeSegmento;

void mostrar_lista_segmentos(t_list* lista) {
    int cantidad_segmentos = list_size(lista);
    char* log_string = malloc(sizeof(char) * (cantidad_segmentos * 100));  // Suponemos un tamaño máximo de 100 caracteres para cada segmento en el log
    char* temp_string = malloc(sizeof(char) * 100);  // Variable temporal para construir cada segmento del log
    strcpy(log_string, "[");

    for (int i = 0; i < cantidad_segmentos; i++) {
        Segmento* segmento = list_get(lista, i);
        sprintf(temp_string, " [B <%i> - L <%i> - V <%i>] ", segmento->base, segmento->limite, segmento->validez);
        strcat(log_string, temp_string);

        if (i != cantidad_segmentos - 1) {
            strcat(log_string, ", ");
        }
    }

    strcat(log_string, "]");
    log_info(memoriaLogger, log_string);

    free(log_string);
    free(temp_string);
}

void mostrar_lista_segmentos_actualizados_por_compactacion(){

    for(int i = 0; i<list_size(listaDeSegmentos); i++){
        Segmento* segmento = list_get(listaDeSegmentos, i);
        log_info(memoriaLogger, BOLDBLUE"PID: "RESET BOLDYELLOW"<%i>"RESET BOLDBLUE" - Segmento: "RESET BOLDYELLOW"<%i>"RESET BOLDBLUE" - Base: "RESET BOLDYELLOW"<%i>"RESET BOLDBLUE" - Tamaño "RESET BOLDYELLOW"<%i>", segmento_get_pid(segmento), segmento_get_id(segmento), segmento_get_base(segmento), segmento_get_tamanio(segmento));
    }

}

bool hueco_mas_grande(Segmento* unSegmento, Segmento* otroSegmento){
    return unSegmento->tamanio > otroSegmento->tamanio;
}


bool hueco_mas_pequenio(Segmento* unSegmento, Segmento* otroSegmento){
    return unSegmento->tamanio < otroSegmento->tamanio;
}


 
bool hay_segmento_libre_de_ese_tamanio(Segmento* unSegmento,Segmento* otroSegmento){
 
 
        if ( segmento_get_tamanio(unSegmento) >= segmento_get_tamanio(otroSegmento) ){
            return true;
        }
        return false;


}

bool segmentos_validez_0(Segmento* unSegmento){
    return (segmento_get_bit_validez(unSegmento) == 0);
}

bool segmentos_validez_1(Segmento* unSegmento){
    return (segmento_get_bit_validez(unSegmento) == 1);
}
 
t_list* segmento_disponibles_del_tamanio(Segmento* nuevoSegmento, t_list* listaDeSegmentoHuecoLibres){
 
    //Semaforos
    t_list* aux = listaDeSegmentoHuecoLibres;
    list_filter_ok(aux,hay_segmento_libre_de_ese_tamanio,nuevoSegmento );
 
    return aux;
}
 
bool es_el_mismo_segmento_por_base(Segmento* unSegmento, Segmento* otroSegmento){
    return segmento_get_base(unSegmento) == segmento_get_base(otroSegmento);
}
 
bool no_hay_fragmentacion_interna(Segmento* nuevoSegmento,Segmento* segmentoReal){
  // tamanio del huevo libre == tamanio_del_segmento
     return (segmento_get_tamanio(segmentoReal) == segmento_get_tamanio(nuevoSegmento));
}
 

 
void administrar_primer_hueco_libre(t_list* huecosLibres, Segmento* nuevoSegmento){
    char* algoritmoAsignacion = memoria_config_get_algoritmo_asignacion(memoriaConfig);
    int index;
    Segmento* libre;
    if( strcmp(algoritmoAsignacion,"FIRST") == 0 ){
        libre = list_get(huecosLibres, 0); //primer hueco libre
    } 
    else if(strcmp(algoritmoAsignacion,"BEST") == 0 ){
        huecosLibres= list_sorted(huecosLibres,hueco_mas_pequenio);
        libre = list_get(huecosLibres,0);//hueco libre mas pequenio
        // FUNCION 2
    } 
    else{             //WORST
        huecosLibres= list_sorted(huecosLibres,hueco_mas_grande);
        libre = list_get(huecosLibres, 0); //hueco libre mas grande
        // FUNCION 3
    }
    //Semaforo
    int index2 = list_get_index(listaDeSegmentos,es_el_mismo_segmento_por_base, libre );
    Segmento* segmentoReal = list_get(listaDeSegmentos,index2); //segmento real del primer hueco encontrado
    bool esElUltimoSegmento = false;
    if(segmento_get_limite(segmentoReal) == memoria_config_get_tamanio_memoria(memoriaConfig)){
        esElUltimoSegmento = true;
    }

    if(no_hay_fragmentacion_interna(nuevoSegmento,segmentoReal)){  //no hay framentacion interna, 
        segmento_set_base(nuevoSegmento, segmento_get_base(segmentoReal));
        segmento_set_limite(nuevoSegmento, segmento_get_limite(segmentoReal));
        segmento_set_bit_validez(nuevoSegmento,1);
        //segmento_set_tamanio(nuevoSegmento);
        list_replace(listaDeSegmentos,index2, nuevoSegmento); 
        log_info(memoriaLogger,ITALIC BLUE "NO HUBO F.INTERNA PID: <%i> - ID: <%i> - BASE: <%i> - TAMAÑO: <%i>", segmento_get_pid(nuevoSegmento), segmento_get_id(nuevoSegmento), segmento_get_base(nuevoSegmento), segmento_get_tamanio(nuevoSegmento));
        //bit_ultimo_seg = 1            //en todos iria en 0 pero en este seria 1 xq tendria el puntero apuntando
    } else {
    
        Segmento* nuevoHuecoLibre = crear_segmento(-1);

        // ACA GUARDO EL SEGMENTO
        segmento_set_base(nuevoSegmento, segmento_get_base(segmentoReal)); // base 128
        uint32_t limite = segmento_get_tamanio(nuevoSegmento) + segmento_get_base(nuevoSegmento) -1 ; // 128 + 64 - 1
        segmento_set_limite(nuevoSegmento, limite); 
        segmento_set_bit_validez(nuevoSegmento,1);
        list_replace(listaDeSegmentos,index2, nuevoSegmento);

        uint32_t baseDelHuecoLibre = segmento_get_limite(nuevoSegmento) + 1;
        uint32_t tamanioDelHuecoLibre = segmento_get_tamanio(segmentoReal) - segmento_get_tamanio(nuevoSegmento);
        log_info(memoriaLogger,BOLD "PID: <%i> - SE CREA SEGMENTO : ID "RESET BOLD RED" <%i> "RESET BOLD" - BASE: "RESET BOLD RED"<%i> "RESET BOLD" - TAMAÑO: "RESET BOLD RED"<%i> "RESET BOLD" - LIMITE "RESET BOLD RED"<%i>", segmento_get_pid(nuevoSegmento), segmento_get_id(nuevoSegmento), segmento_get_base(nuevoSegmento), segmento_get_tamanio(nuevoSegmento), segmento_get_limite(nuevoSegmento));

        // ACA TRABAJO CON LA FRAGMENTACION INTERNA QUE GENERA
        segmento_set_base(nuevoHuecoLibre,baseDelHuecoLibre);
        segmento_set_tamanio(nuevoHuecoLibre,tamanioDelHuecoLibre);
        uint32_t limiteHuecoLibre = baseDelHuecoLibre + tamanioDelHuecoLibre -1 ;
        segmento_set_limite(nuevoHuecoLibre,limiteHuecoLibre);

        segmento_set_bit_validez(nuevoHuecoLibre,0);
        list_add_in_index(listaDeSegmentos,index2+1,nuevoHuecoLibre);
        log_info(memoriaLogger,BOLD BLUE "SE CREA HUECO LIBRE: BASE: "RESET BOLD YELLOW "<%i>" RESET BOLD BLUE" - TAMAÑO: "RESET BOLD YELLOW" <%i>", baseDelHuecoLibre, tamanioDelHuecoLibre);

        
    }
}
 
 
uint8_t administrar_nuevo_segmento(Segmento* nuevoSegmento){
    
    t_list* listaDeHuecosLibres = listaDeSegmentos; 
 
    listaDeHuecosLibres = list_filter(listaDeHuecosLibres,segmentos_validez_0);
    log_info(memoriaLogger,GREEN BOLD "CANTIDAD DE HUECOS LIBRES  "RESET BOLD YELLOW " <%i>", list_size(listaDeHuecosLibres) );
    listaDeHuecosLibres = list_filter_ok(listaDeHuecosLibres,hay_segmento_libre_de_ese_tamanio,nuevoSegmento);
    log_info(memoriaLogger,GREEN BOLD "CANTIDAD HUECOS LIBRES QUE PUEDA ENTRAR EL SEGMENTO " RESET BOLD YELLOW " <%i>", list_size(listaDeHuecosLibres) );


    if( list_is_empty(listaDeHuecosLibres) ){
       return HEADER_Compactacion;

    } else {
        
        administrar_primer_hueco_libre(listaDeHuecosLibres, nuevoSegmento);

        return HEADER_create_segment;

    }
 
}


bool es_el_mismo_segmento_pid_id(Segmento* unSegmento, Segmento* otroSegmento){

return ( segmento_get_id(unSegmento) == segmento_get_id(otroSegmento) ) && ( segmento_get_pid(unSegmento) == segmento_get_pid(otroSegmento) );


}

int segmento_anterior_esta_libre(int index){
    Segmento* segmentoAnterior = list_get(listaDeSegmentos, index - 1);

    return segmento_get_bit_validez(segmentoAnterior);
}


int segmento_siguiente_esta_libre(int index){
   

    Segmento* segmentoAnterior = list_get(listaDeSegmentos, index + 1);
    return segmento_get_bit_validez(segmentoAnterior);
   
}

Segmento* consolidar_segmentos(Segmento* unSegmento, Segmento* segSiguiente){
    
    // sumas sus tamanios
    segmento_set_tamanio(unSegmento, segmento_get_tamanio(unSegmento) + segmento_get_tamanio(segSiguiente) );
    segmento_set_limite(unSegmento,segmento_get_limite(segSiguiente));
    segmento_set_bit_validez(unSegmento, 0);
    return unSegmento;

}


void eliminar_segmento_memoria(Segmento* segmentoAEliminar){
    
    int index = list_get_index(listaDeSegmentos,es_el_mismo_segmento_pid_id, segmentoAEliminar ); 
    Segmento* segmentoRealAModificar = list_get(list_filter_ok(listaDeSegmentos,es_el_mismo_segmento_pid_id,segmentoAEliminar),0);
    sumar_memoriaRecuperada_a_tamMemoriaActual(segmento_get_tamanio(segmentoRealAModificar));
    log_info(memoriaLogger,BOLDBLUE  "PID: "RESET BOLDRED "<%i>"RESET BOLDBLUE" - Eliminar Segmento: "RESET BOLDRED"<%i>"RESET BOLDBLUE" - Base: "RESET BOLDRED"<%i>"RESET BOLDBLUE" - TAMAÑO: "RESET BOLDRED"<%i>",segmento_get_pid(segmentoRealAModificar), segmento_get_id(segmentoRealAModificar), segmento_get_base(segmentoRealAModificar), segmento_get_tamanio(segmentoRealAModificar) );

    if(segmentoRealAModificar->pid == -1) return 0;
    segmento_set_bit_validez(segmentoRealAModificar, 0);

    if(index+1 >= list_size(listaDeSegmentos) ){ // ES EL ULTIMO SEGMENTO DE LA MEMORIA
        if(segmento_anterior_esta_libre(index) == 0){

            Segmento* segmentoAnterior =list_get(listaDeSegmentos, index - 1);    
            Segmento* segmentoConsolidado  = consolidar_segmentos(segmentoAnterior, segmentoRealAModificar);
            list_replace(listaDeSegmentos,index - 1,segmentoConsolidado);
            list_remove(listaDeSegmentos, index);

            
        } else {
            list_replace(listaDeSegmentos,index,segmentoRealAModificar); 
        } //caso donde el anterior es el segmento 0
        

        

    } else { // ACA SI EL SEGMENTO ESTA ENTRE MEDIO DE OTROS SEGMENTOS

        if(segmento_anterior_esta_libre(index) == 0){
            Segmento* segmentoAnterior = list_get(listaDeSegmentos, index - 1);    
            Segmento* segmentoConsolidado  = consolidar_segmentos(segmentoAnterior, segmentoRealAModificar);
            list_replace(listaDeSegmentos,index - 1,segmentoConsolidado);
            list_remove(listaDeSegmentos, index);
            index = index -1;
        } 
        if(segmento_siguiente_esta_libre(index) == 0){
            Segmento* aux = list_get(listaDeSegmentos,index);
            Segmento* segmentoSiguiente = list_get(listaDeSegmentos, index+1);    
            Segmento* segmentoConsolidado  = consolidar_segmentos(aux, segmentoSiguiente);
            list_replace(listaDeSegmentos,index,segmentoConsolidado);
            list_remove(listaDeSegmentos, index + 1);

        }  
        
        


    }

}



void liberar_tabla_segmentos(int pid){
    t_list* tablaDeSegmentoAELiminar = obtener_tabla_de_segmentos_por_pid(pid);

    for (int i = 0; i < list_size(tablaDeSegmentoAELiminar); i++)
    {   
        Segmento* segmentoAEliminar = list_get(tablaDeSegmentoAELiminar,i); 

        if(segmentoAEliminar->pid != -1 && segmentoAEliminar->validez != 0){
        eliminar_segmento_memoria(segmentoAEliminar);  

        }
    }
     
    list_destroy(tablaDeSegmentoAELiminar);
}

bool el_limite_del_segmento_anterior_es_igual_base_segmento_actual(Segmento* segmentoActual,Segmento* segmentoAnterior ){

return (segmentoAnterior->limite) +1 == segmentoActual->base;
}

bool es_el_ultimo_segmento_lista(int index){
    return index+1 >= list_size(listaDeSegmentos);
}
void iniciar_compactacion() {
    Segmento* segmentoActual;
    Segmento* segmentoAnterior;

    uint32_t retardoInstruccion = memoria_config_get_retardo_compactacion(memoriaConfig);
    intervalo_de_pausa(retardoInstruccion);

    for (int i = 1; i < list_size(listaDeSegmentos); i++) {
        segmentoActual = list_get(listaDeSegmentos, i);
        segmentoAnterior = list_get(listaDeSegmentos, i - 1);


        if (es_el_ultimo_segmento_lista(i) && segmentoActual->validez == 0) {
             
            segmento_set_base(segmentoActual, segmento_get_limite(segmentoAnterior)+1);
            segmento_set_tamanio(segmentoActual, segmento_get_limite(segmentoActual) - segmento_get_base(segmentoActual) + 1);
            list_replace(listaDeSegmentos, i, segmentoActual);
          
        } else {
            if (segmento_anterior_esta_libre(i) == 0) {
                if (es_el_ultimo_segmento_lista(i)) {
                     memmove(memoriaPrincipal + (size_t)segmento_get_base(segmentoAnterior),
                     memoriaPrincipal + (size_t)segmento_get_base(segmentoActual),
                     (size_t)segmento_get_tamanio(segmentoActual));

                    segmento_set_limite(segmentoAnterior, segmento_get_base(segmentoAnterior) + segmento_get_tamanio(segmentoActual) -1 );
                    segmento_set_tamanio(segmentoAnterior, segmento_get_tamanio(segmentoActual));
                    segmento_set_base(segmentoActual, segmento_get_limite(segmentoAnterior) + 1);
                    segmento_set_id(segmentoAnterior, segmento_get_id(segmentoActual));
                    segmento_set_pid(segmentoAnterior, segmento_get_pid(segmentoActual));
                    segmento_set_id(segmentoActual, -1);
                    segmento_set_pid(segmentoActual, -1);
                    segmento_set_bit_validez(segmentoAnterior, 1);
                    segmento_set_tamanio(segmentoActual, 1 + segmento_get_limite(segmentoActual)- segmento_get_base(segmentoActual));
                    segmento_set_bit_validez(segmentoActual, 0);
                } else {
                   
                    memmove(memoriaPrincipal + (size_t)segmento_get_base(segmentoAnterior),
                    memoriaPrincipal + (size_t)segmento_get_base(segmentoActual),
                    (size_t)segmento_get_tamanio(segmentoActual));

                    segmento_set_base(segmentoActual, segmento_get_base(segmentoAnterior));
                    segmento_set_limite(segmentoActual, segmento_get_base(segmentoActual) +  segmento_get_tamanio(segmentoActual) -1  );
                    segmento_set_tamanio(segmentoActual, segmento_get_limite(segmentoActual) - segmento_get_base(segmentoActual) + 1);
                    list_replace(listaDeSegmentos, i - 1, segmentoActual);
                    list_remove(listaDeSegmentos, i);
                    i = i - 1;

                  
                }
            } else if (!el_limite_del_segmento_anterior_es_igual_base_segmento_actual(segmentoActual, segmentoAnterior)) {
                // Actualizar el contenido en el void*
                memmove(memoriaPrincipal + (size_t)segmento_get_limite(segmentoAnterior) + 1,
                       (size_t)memoriaPrincipal + (size_t)segmento_get_base(segmentoActual),
                       (size_t)segmento_get_tamanio(segmentoActual));

                segmento_set_base(segmentoActual, segmento_get_limite(segmentoAnterior) + 1);
                segmento_set_limite(segmentoActual, segmento_get_base(segmentoActual) +  segmento_get_tamanio(segmentoActual) );
                segmento_set_tamanio(segmentoActual, segmento_get_limite(segmentoActual) - segmento_get_base(segmentoActual));
                list_replace(listaDeSegmentos, i, segmentoActual);

            }
          
        }
    }
}