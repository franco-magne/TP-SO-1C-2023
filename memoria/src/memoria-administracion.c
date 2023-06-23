#include <../include/memoria-administracion.h>

extern t_list* listaDeSegmentos;
extern t_log *memoriaLogger;
extern t_memoria_config* memoriaConfig;

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

bool segmento_mas_grande(Segmento* unSegmento, Segmento* otroSegmento){
    return unSegmento->tamanio > otroSegmento->tamanio;
}


bool segmento_mas_pequenio(Segmento* unSegmento, Segmento* otroSegmento){
    return unSegmento->tamanio < otroSegmento->tamanio;
}

/*int algoritmo_best(t_list* unaLista){
    Segmento* aux = crear_segmento(1);
    int index = list_get_index(unaLista, segmento_mas_pequenio, aux);
    free(aux);
    return index;
}
*/
int algoritmo_worst(t_list* unaLista){
    Segmento* menorSegmento = NULL;
    int menorIndex = -1;

    for (int i = 0; i < list_size(unaLista); i++) {
        Segmento* segmento = list_get(unaLista, i);

        if (menorSegmento == NULL || segmento->tamanio > menorSegmento->tamanio) {
            menorSegmento = segmento;
            menorIndex = i;
        }
    }

    return menorIndex;
}

int algoritmo_best(t_list* unaLista, uint32_t tamSegmento) {
    int mejorIndex = -1;
    uint32_t mejorDiferencia = 4294967295;   //tamanio max de un int

    for (int i = 0; i < list_size(unaLista); i++) {
        Segmento* segmento = list_get(unaLista, i);

        if (!segmento->validez   && segmento->tamanio >= tamSegmento) {
            int diferencia = segmento->tamanio - tamSegmento;
            if (diferencia < mejorDiferencia) {
                mejorIndex = i;
                mejorDiferencia = diferencia;
            }
        }
    }

    return mejorIndex;
}



t_list* list_filter_ok(t_list* lista, bool (*condition)(void*, void*), void* argumento) {
    t_list* resultado = list_create();
 
    for (int i = 0; i < lista->elements_count; i++) {
        void* elemento = list_get(lista, i);
        if (condition(elemento, argumento)) {
            list_add(resultado, elemento);
        }
    }
 
    return resultado;
}
 
 
bool hay_segmento_libre_de_ese_tamanio(Segmento* unSegmento,Segmento* otroSegmento){
 
 
        if ( segmento_get_tamanio(unSegmento) > segmento_get_tamanio(otroSegmento) ){
            return true;
        }
        return false;


}

bool segmentos_validez_0(Segmento* unSegmento){
    return (segmento_get_bit_validez(unSegmento) == 0);
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
        index = algoritmo_best(huecosLibres,segmento_get_tamanio(nuevoSegmento));
        libre = list_get(huecosLibres,index);//hueco libre mas pequenio
        // FUNCION 2
    } 
    else{             //WORST
        index = algoritmo_worst(huecosLibres);
        libre = list_get(huecosLibres, index); //hueco libre mas grande
        // FUNCION 3
    }
    //Semaforo
    int index2 = list_get_index(listaDeSegmentos,es_el_mismo_segmento_por_base, libre );
    Segmento* segmentoReal = list_get(listaDeSegmentos,index2); //segmento real del primer hueco encontrado
    
    if(no_hay_fragmentacion_interna(nuevoSegmento,segmentoReal)){  //no hay framentacion interna, 
        segmento_set_base(nuevoSegmento, segmento_get_base(segmentoReal));
        segmento_set_limite(nuevoSegmento, segmento_get_limite(segmentoReal));
        segmento_set_bit_validez(nuevoSegmento,1);
        list_replace(listaDeSegmentos,index2, nuevoSegmento); 
        log_info(memoriaLogger,"no hubo F.interna PID: <%i> - Crear Segmento: <%i> - Base: <%i> - TAMAÑO: <%i>", segmento_get_pid(nuevoSegmento), segmento_get_id(nuevoSegmento), segmento_get_base(nuevoSegmento), segmento_get_tamanio(nuevoSegmento));
        //bit_ultimo_seg = 1            //en todos iria en 0 pero en este seria 1 xq tendria el puntero apuntando
    } else {
    
        Segmento* nuevoHuecoLibre = crear_segmento(-1);

        // ACA GUARDO EL SEGMENTO
        segmento_set_base(nuevoSegmento, segmento_get_base(segmentoReal));
        uint32_t limite = segmento_get_tamanio(nuevoSegmento) + segmento_get_base(nuevoSegmento);
        segmento_set_limite(nuevoSegmento, limite); //tamanio viene x default
        segmento_set_bit_validez(nuevoSegmento,1);
        list_replace(listaDeSegmentos,index2, nuevoSegmento);

        uint32_t tamanioDelHuecoLibre = segmento_get_tamanio(segmentoReal) - segmento_get_tamanio(nuevoSegmento);
        uint32_t baseDelHuecoLibre = segmento_get_limite(nuevoSegmento);            //segmento_get_limite(segmentoReal) ;
        log_info(memoriaLogger,"PID: <%i> - Crear Segmento: <%i> - Base: <%i> - TAMAÑO: <%i> - LIMITE <%i>", segmento_get_pid(nuevoSegmento), segmento_get_id(nuevoSegmento), segmento_get_base(nuevoSegmento), segmento_get_tamanio(nuevoSegmento), segmento_get_limite(nuevoSegmento));

        // ACA TRABAJO CON LA FRAGMENTACION INTERNA QUE GENERA
        segmento_set_base(nuevoHuecoLibre,baseDelHuecoLibre);
        segmento_set_tamanio(nuevoHuecoLibre,tamanioDelHuecoLibre);
        uint32_t limiteHuecoLibre = tamanioDelHuecoLibre + baseDelHuecoLibre;
        segmento_set_limite(nuevoHuecoLibre,limiteHuecoLibre); //coincide con el limite de segmentoReal
        segmento_set_bit_validez(nuevoHuecoLibre,0);
        list_add_in_index(listaDeSegmentos,index2+1,nuevoHuecoLibre);
        log_info(memoriaLogger,"Crear Hueco Libre: Base: <%i> - TAMAÑO: <%i>", baseDelHuecoLibre, tamanioDelHuecoLibre);

        
    }
}
 
 
void administrar_nuevo_segmento(Segmento* nuevoSegmento){
    

    
   // t_list* listaDeHuecosLibres = listaDeSegmentos;
  

    t_list* listaDeHuecosLibres = listaDeSegmentos; //t_list* listaDeHuecosLibres = list_filter(listaDeSegmentos,segmentos_validez_0);
    //listaDeHuecosLibres = list_filter(listaDeSegmentos,segmentos_validez_0);
    //listaDeHuecosLibres = segmento_disponibles_del_tamanio(nuevoSegmento, listaDeHuecosLibres);
 
    listaDeHuecosLibres = list_filter(listaDeHuecosLibres,segmentos_validez_0);
    listaDeHuecosLibres = list_filter_ok(listaDeHuecosLibres,hay_segmento_libre_de_ese_tamanio,nuevoSegmento);
    

    if( list_is_empty(listaDeHuecosLibres) ){
        // Hay que hacer compactacion
        
    } else {
        
        administrar_primer_hueco_libre(listaDeHuecosLibres, nuevoSegmento);
    // Aca se elije el algoritmo dependiendo como 
    // Hagamos uno de test
 
 
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
    log_info(memoriaLogger, "CONSOLIDAN SEGMENTO (1) ID <%i> - SEGMENTO (2) ID <%i>", segmento_get_id(unSegmento),segmento_get_id(segSiguiente));
    return unSegmento;

}

void eliminar_segmento_memoria(Segmento* segmentoAEliminar){
    
    int index = list_get_index(listaDeSegmentos,es_el_mismo_segmento_pid_id, segmentoAEliminar ); 
    Segmento* segmentoRealAModificar = list_get(listaDeSegmentos,index);
    segmento_set_bit_validez(segmentoRealAModificar, 0);
    log_info(memoriaLogger, "PROCESO: PID <%i> -  SEGMENTO <%i> - VALIDEZ <%i> ", segmento_get_pid(segmentoRealAModificar), segmento_get_id(segmentoRealAModificar), segmento_get_bit_validez(segmentoRealAModificar));

    if(es_el_ultimo_elemento(listaDeSegmentos,segmentoRealAModificar)){ // ES EL ULTIMO SEGMENTO DE LA MEMORIA
        if(segmento_anterior_esta_libre(index) == 0){

            Segmento* segmentoAnterior =list_get(listaDeSegmentos, index - 1);    
            Segmento* segmentoConsolidado  = consolidar_segmentos(segmentoAnterior, segmentoRealAModificar);
            list_replace(listaDeSegmentos,index - 1,segmentoConsolidado);
            list_remove(listaDeSegmentos, index);
            
        }  //caso donde el anterior es el segmento 0
        
        list_replace(listaDeSegmentos,index,segmentoRealAModificar); 

        

    } else { // ACA SI EL SEGMENTO ESTA ENTRE MEDIO DE OTROS SEGMENTOS

        if(segmento_anterior_esta_libre(index) == 0){
            Segmento* segmentoAnterior = list_get(listaDeSegmentos, index - 1);    
            Segmento* segmentoConsolidado  = consolidar_segmentos(segmentoAnterior, segmentoRealAModificar);
            list_replace(listaDeSegmentos,index - 1,segmentoConsolidado);
            list_remove(listaDeSegmentos, index);
       
        } 
        if(segmento_siguiente_esta_libre(index) == 0){
            Segmento* segmentoSiguiente = list_get(listaDeSegmentos, index+1);    
            Segmento* segmentoConsolidado  = consolidar_segmentos(segmentoRealAModificar, segmentoSiguiente);
            list_replace(listaDeSegmentos,index,segmentoConsolidado);
            list_remove(listaDeSegmentos, index + 1);
        }  
        
        list_replace(listaDeSegmentos,index,segmentoRealAModificar);
        


    }

}

void liberar_tabla_segmentos(int pid){
    t_list* tablaDeSegmentoAELiminar = obtener_tabla_de_segmentos_por_pid(pid);
    
    for (int i = 0; i < list_size(tablaDeSegmentoAELiminar); i++)
    {
        Segmento* segmentoAEliminar = list_get(tablaDeSegmentoAELiminar,i); 
        eliminar_segmento_memoria(segmentoAEliminar);    
    }
     
    list_destroy(tablaDeSegmentoAELiminar);
}


