#include <../include/memoria-atender-cpu.h>
extern pthread_mutex_t mutexMemoriaData;
extern t_log *memoriaLogger;
extern t_memoria_config* memoriaConfig;
extern uint32_t tamActualMemoria;
extern Segmento* segCompartido;

void atender_peticiones_cpu(int socketCpu) {
    uint8_t header;
    for(;;){
        header = stream_recv_header(socketCpu);
        pthread_mutex_lock(&mutexMemoriaData);
        t_buffer* buffer = buffer_create();
        stream_recv_buffer(socketCpu, buffer);
        switch (header){
        case HEADER_marco :{
            //marco es la direccion Fisica
            log_info(memoriaLogger, "\e[1;93mPetición de marco\e[0m");
            uint32_t id_segmento;
            int pid;
            buffer_unpack(buffer, &id_segmento, sizeof(id_segmento));
            buffer_unpack(buffer, &pid, sizeof(pid));
            
            log_info(memoriaLogger, "Se quiere la dirección física del segmento <%i>",id_segmento);
            
            uint32_t marco = obtener_marco(pid, id_segmento);
            
            t_buffer* buffer_rta = buffer_create();
            buffer_pack(buffer_rta, &marco, sizeof(marco));
            stream_send_buffer(socket, HEADER_marco, buffer_rta);
            buffer_destroy(buffer_rta);
            log_info(memoriaLogger, "Se enviá la dirección física [%d]", marco);
            
            //buffer_destroy(buffer);
        }
        break;
        case HEADER_move_in :{ //"leer"

        
            //buffer_destroy(buffer);
        }
        break;
        case HEADER_move_out : {//"escribir"
            log_info(memoriaLogger, "\e[1;93mPetición de escritura\e[0m");
            
            uint32_t direccFisica;
            t_registro contenidoAEscribir;
            buffer_unpack(buffer, &direccFisica, sizeof(direccFisica));
            buffer_unpack(buffer, &contenidoAEscribir, sizeof(contenidoAEscribir));
            
            //validar que no supere el espacio disponible?
            memcpy((void*)&direccFisica,(void*)&contenidoAEscribir,sizeof(contenidoAEscribir));





            
            break;
            //buffer_destroy(buffer);
        }
        break;
        default:
        break;
        }
    }
}
/*
log_info(memoriaData->memoriaLogger, "\e[1;93mPetición de escritura\e[0m");
                buffer_unpack(buffer, &direccionFisica, sizeof(direccionFisica));
                buffer_unpack(buffer, &valor, sizeof(valor));
                memcpy((void*)(memoriaPrincipal + direccionFisica), (void*)&valor, sizeof(valor));
                __actualizar_pagina(direccionFisica, true, memoriaData);
                log_info(memoriaData->memoriaLogger, "Se escribio el valor [%d] en la dirección física [%d]", *((uint32_t*)(memoriaPrincipal + direccionFisica)), direccionFisica);
                break;

*/

// actualizar pagina
/*static void __actualizar_pagina(uint32_t direccionFisica, bool esEscritura, t_memoria_data_holder* memoriaData) {
    int nroPagina = obtener_pagina_de_direccion_fisica(direccionFisica, memoriaData);
    int nroTablaNivel2 = obtener_tabla_de_nivel_2_pagina(nroPagina, memoriaData);
    if (esEscritura)
        actualizar_escritura_pagina(nroPagina, nroTablaNivel2, memoriaData);
    else
        actualizar_lectura_pagina(nroPagina, nroTablaNivel2, memoriaData);
    __loggear_paginas_en_memoria_del_proceso(nroTablaNivel2, memoriaData);
}

*/
uint32_t obtener_direccFisica(int pid, uint32_t id_segmento){
    Procesos* miProceso = obtener_proceso_por_pid(pid);
    Segmento* segBuscado = obtener_segmento_por_id(miProceso, id_segmento);

    uint32_t base = segmento_get_base(segBuscado);
    uint32_t limite = segmento_get_limite(segBuscado);

    return base + limite;
}



