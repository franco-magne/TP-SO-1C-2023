#include <../include/memoria-administracion.h>

t_list* memoriaRam;



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
 
 
bool hay_segmento_libre_de_ese_tamanio(Segmento* unSegmento,Segmento* otroSegento){
 
    if(segmento_get_bit_validez(unSegmento) != 0){
 
        if ( segmento_get_tamanio(otroSegento) > segmento_get_tamanio(unSegmento) ){
            return true;
        }
        return false;

    }
    
    return false;
}
 
t_list* segmento_disponibles_del_tamanio(Segmento* nuevoSegmento){
 
    //Semaforos
    t_list* aux = memoriaRam;
    list_filter_ok(aux,hay_segmento_libre_de_ese_tamanio,nuevoSegmento );
 
    return aux;
}
 
bool es_el_mismo_segmento_por_base(Segmento* unSegmento, Segmento* otroSegmento){
    return segmento_get_base(unSegmento) == segmento_get_base(otroSegmento);
}
 
bool sobra_espacio_del_hueco_libre(Segmento* nuevoSegmento,Segmento* segmentoReal){
  // tamanio del huevo libre == tamanio_del_segmento
     return (segmento_get_tamanio(segmentoReal) == segmento_get_tamanio(nuevoSegmento));
}
 
 
void administrar_primer_hueco_libre(t_list* huecosLibres, Segmento* nuevoSegmento){
    Segmento* libre = list_get(huecosLibres, 0);
    //Semaforo
    int index = list_get_index(memoriaRam,es_el_mismo_segmento_por_base, libre );
    Segmento* segmentoReal = list_get(memoriaRam,index);
    if(sobra_espacio_del_hueco_libre(nuevoSegmento,segmentoReal)){  //no hay framentacion interna, 
        segmento_set_base(nuevoSegmento, segmento_get_base(segmentoReal));
        segmento_set_limite(nuevoSegmento, segmento_get_limite(segmentoReal));
        segmento_set_bit_validez(nuevoSegmento,1);
        list_replace(memoriaRam,index, nuevoSegmento);
        //bit_ultimo_seg = 1            //en todos iria en 0 pero en este seria 1 xq tendria el puntero apuntando
    } else {
    
    Segmento* nuevoHuecoLibre = crear_segmento(-1);
    uint32_t tamanioDelSegmento = segmento_get_tamanio(segmentoReal) - segmento_get_tamanio(nuevoSegmento);
    uint32_t baseDelNuevoSegmento = segmento_get_limite(segmentoReal) ;

    // ACA TRABAJO CON LA FRAGMENTACION INTERNA QUE GENERA
    segmento_set_base(nuevoHuecoLibre,baseDelNuevoSegmento);
    segmento_set_tamanio(nuevoHuecoLibre,tamanioDelSegmento);
    segmento_set_bit_validez(nuevoHuecoLibre,0);
    list_add_in_index(memoriaRam,index+1,nuevoHuecoLibre);

    // ACA GUARDO EL SEGMENTO
    segmento_set_base(nuevoSegmento, segmento_get_base(segmentoReal));
    segmento_set_limite(nuevoSegmento, segmento_get_limite(segmentoReal));
    segmento_set_bit_validez(nuevoSegmento,1);
    list_replace(memoriaRam,index, nuevoSegmento);
        
    }
 

}
 
 
void administrar_nuevo_segmento(Segmento* nuevoSegmento){
 
    t_list* listaDeHuecosLibres = segmento_disponibles_del_tamanio(nuevoSegmento);
 
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

bool segmento_anterior_esta_libre(int index){
    Segmento* segmentoAnterior = list_get(memoriaRam, index - 1);

    return segmento_get_bit_validez(segmentoAnterior);
}


bool segmento_siguiente_esta_libre(int index){
    Segmento* segmentoAnterior = list_get(memoriaRam, index + 1);

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

    int index = list_get_index(memoriaRam,es_el_mismo_segmento_pid_id, segmentoAEliminar );
    segmento_set_bit_validez(segmentoAEliminar, 0);
    if(es_el_ultimo_elemento(memoriaRam,segmentoAEliminar)){ // ES EL ULTIMO SEGMENTO DE LA MEMORIA

        if(segmento_anterior_esta_libre(index) == 0){

        Segmento* segmentoAnterior =list_get(memoriaRam, index - 1);    
        Segmento* segmentoConsolidado  = consolidar_segmentos(segmentoAnterior, segmentoAEliminar);
        list_replace(memoriaRam,index - 1,segmentoConsolidado);
        list_remove(memoriaRam, index);
        
        } else {
        
         list_replace(memoriaRam,index,segmentoAEliminar);


        }

    } else { // ACA SI EL SEGMENTO ESTA ENTRE MEDIO DE OTROS SEGMENTOS

        if(segmento_anterior_esta_libre(index) == 0){ 

        Segmento* segmentoAnterior =list_get(memoriaRam, index - 1);    
        Segmento* segmentoConsolidado  = consolidar_segmentos(segmentoAnterior, segmentoAEliminar);
        list_replace(memoriaRam,index - 1,segmentoConsolidado);
        list_remove(memoriaRam, index);
       
        } else if(segmento_siguiente_esta_libre(index) == 0){
            Segmento* segmentoSiguiente = list_get(memoriaRam, index+1);    
            Segmento* segmentoConsolidado  = consolidar_segmentos(segmentoAEliminar, segmentoSiguiente);
            list_replace(memoriaRam,index,segmentoConsolidado);
            list_remove(memoriaRam, index + 1);
        } else {
            list_replace(memoriaRam,index,segmentoAEliminar);
        }


    }

}