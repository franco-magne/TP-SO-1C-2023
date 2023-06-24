#include <../include/kernel-cpu-adapter.h> 

void cpu_adapter_enviar_pcb_a_cpu(t_pcb* pcbAEnviar, uint8_t header, t_kernel_config* kernelConfig, t_log* kernelLogger)
{
    uint32_t pidAEnviar = pcb_get_pid(pcbAEnviar);
    uint32_t pcAEnviar = pcb_get_program_counter(pcbAEnviar);

    t_registros_cpu* registrosCpu = pcb_get_registros_cpu(pcbAEnviar); 

    //Empaquetamos pid y pc
    t_buffer* bufferPcbAEjecutar = buffer_create();
    buffer_pack(bufferPcbAEjecutar, &pidAEnviar, sizeof(pidAEnviar));
    buffer_pack(bufferPcbAEjecutar, &pcAEnviar, sizeof(pcAEnviar));

    char* registroAx =  pcb_get_registros_cpu(pcbAEnviar)->registroAx;
    char* registroBx =  pcb_get_registros_cpu(pcbAEnviar)->registroBx;
    char* registroCx =  pcb_get_registros_cpu(pcbAEnviar)->registroCx;
    char* registroDx =  pcb_get_registros_cpu(pcbAEnviar)->registroDx;

    //Empaquetamos los registros
    buffer_pack_string(bufferPcbAEjecutar, registroAx );
    buffer_pack_string(bufferPcbAEjecutar, registroBx );
    buffer_pack_string(bufferPcbAEjecutar, registroCx );
    buffer_pack_string(bufferPcbAEjecutar, registroDx );

    //stream_send_empty_buffer(kernel_config_get_socket_dispatch_cpu(kernelConfig), header);
    stream_send_buffer(kernel_config_get_socket_dispatch_cpu(kernelConfig), header, bufferPcbAEjecutar);
    stream_send_buffer(kernel_config_get_socket_dispatch_cpu(kernelConfig), HEADER_lista_instrucciones, pcb_get_instrucciones_buffer(pcbAEnviar));

    buffer_destroy(bufferPcbAEjecutar);
}

t_pcb* cpu_adapter_recibir_pcb_actualizado_de_cpu(t_pcb* pcbAActualizar, uint8_t cpuResponse, t_kernel_config* kernelConfig, t_log* kernelLogger)
{
    uint32_t pidRecibido = 0;
    uint32_t programCounterActualizado = 0;
    char* registroAxActualizado = NULL, registroBxActualizado = NULL, registroCxActualizado = NULL, registroDxActualizado = NULL;
    uint32_t cantidadUnidadesTiemposIo = 0;
    // MEMORIA
    uint32_t id_de_segmento;
    uint32_t tamanio_de_segmento;
    // FILE SYSTEM
    char* nombreArchivo;
    uint32_t tamanioArchivo;
    uint32_t punteroArchivo;
    uint32_t direccionFisicaArchivo;

    t_buffer* bufferPcb = buffer_create();

    stream_recv_buffer(kernel_config_get_socket_dispatch_cpu(kernelConfig), bufferPcb);
    buffer_unpack(bufferPcb, &pidRecibido, sizeof(pidRecibido));
    buffer_unpack(bufferPcb, &programCounterActualizado, sizeof(programCounterActualizado));
    

    //desempaquetar regs 
    registroAxActualizado = buffer_unpack_string(bufferPcb);
    registroBxActualizado = buffer_unpack_string(bufferPcb);
    registroCxActualizado = buffer_unpack_string(bufferPcb);
    registroDxActualizado = buffer_unpack_string(bufferPcb);


    switch(cpuResponse){
        case HEADER_proceso_bloqueado : 
        
        uint32_t unidadesDeTrabajo;
        buffer_unpack(bufferPcb, &unidadesDeTrabajo, sizeof(unidadesDeTrabajo));
        pcb_set_tiempoIO(pcbAActualizar,unidadesDeTrabajo);
        break;

        case HEADER_proceso_pedir_recurso:
        case HEADER_proceso_devolver_recurso: 
        char* recursoAux = buffer_unpack_string(bufferPcb);
        pcb_set_recurso_utilizado(pcbAActualizar, recursoAux);
        break;

        case HEADER_create_segment:
      

        buffer_unpack(bufferPcb, &id_de_segmento, sizeof(id_de_segmento));
        buffer_unpack(bufferPcb, &tamanio_de_segmento, sizeof(tamanio_de_segmento));

        t_segmento* unSegmento = segmento_create(id_de_segmento, tamanio_de_segmento);
    
        pcb_set_lista_de_segmentos(pcbAActualizar,unSegmento);

        //segmento_destroy(unSegmento);

        break;


        case HEADER_delete_segment:
        
        buffer_unpack(bufferPcb, &id_de_segmento, sizeof(id_de_segmento));
        log_info(kernelLogger, "ID <%i> Segmento eliminar ", id_de_segmento);
        modificar_victima_lista_segmento(pcbAActualizar,id_de_segmento, true);


        break;

        case HEADER_f_open:
        nombreArchivo = buffer_unpack_string(bufferPcb);
        t_pcb_archivo *nuevoArchivo = archivo_create_pcb(nombreArchivo);
        pcb_add_lista_de_archivos(pcbAActualizar, nuevoArchivo);
        
        break;

        case HEADER_f_close:
        nombreArchivo = buffer_unpack_string(bufferPcb);
        modificar_victima_archivo(pcb_get_lista_de_archivos_abiertos(pcbAActualizar),nombreArchivo,true);

        break;

        case HEADER_f_truncate:

        nombreArchivo = buffer_unpack_string(bufferPcb);
        buffer_unpack(bufferPcb, &tamanioArchivo, sizeof(tamanioArchivo));

        int index = index_de_archivo_pcb(pcb_get_lista_de_archivos_abiertos(pcbAActualizar),nombreArchivo);
        t_pcb_archivo* archivoTruncate = list_get(pcb_get_lista_de_archivos_abiertos(pcbAActualizar), index);
        archivo_pcb_set_tamanio_archivo(archivoTruncate,tamanioArchivo);
        archivo_pcb_set_victima(archivoTruncate,true);
        list_replace(pcb_get_lista_de_archivos_abiertos(pcbAActualizar),index,archivoTruncate);
        
        break;

        case HEADER_f_seek:

        nombreArchivo = buffer_unpack_string(bufferPcb);
        buffer_unpack(bufferPcb, &punteroArchivo, sizeof(punteroArchivo));

        index = index_de_archivo_pcb(pcb_get_lista_de_archivos_abiertos(pcbAActualizar),nombreArchivo);
        t_pcb_archivo* archivoFSeek = list_get(pcb_get_lista_de_archivos_abiertos(pcbAActualizar), index);
        archivo_pcb_set_puntero_archivo(archivoFSeek,punteroArchivo);
        archivo_pcb_set_victima(archivoFSeek,true);
        list_replace(pcb_get_lista_de_archivos_abiertos(pcbAActualizar),index,archivoFSeek);

        break;

        case HEADER_f_read:
        case HEADER_f_write:

        nombreArchivo = buffer_unpack_string(bufferPcb);
        buffer_unpack(bufferPcb, &punteroArchivo, sizeof(punteroArchivo));
        buffer_unpack(bufferPcb, &direccionFisicaArchivo, sizeof(direccionFisicaArchivo));

        index = index_de_archivo_pcb(pcb_get_lista_de_archivos_abiertos(pcbAActualizar),nombreArchivo);
        t_pcb_archivo* archivoFRW = list_get(pcb_get_lista_de_archivos_abiertos(pcbAActualizar), index);
        archivo_pcb_set_puntero_archivo(archivoFRW,punteroArchivo);
        archivo_pcb_set_victima(archivoFRW,true);
        archivo_pcb_set_direccion_fisica(archivoFRW,direccionFisicaArchivo);
        list_replace(pcb_get_lista_de_archivos_abiertos(pcbAActualizar),index,archivoFRW);
        break;
    }
    
   if (pidRecibido == pcb_get_pid(pcbAActualizar)) {
        
        switch(cpuResponse){
            case HEADER_proceso_desalojado:
            case HEADER_proceso_bloqueado:
            case HEADER_proceso_pedir_recurso:
            case HEADER_proceso_devolver_recurso:
            case HEADER_create_segment:
            case HEADER_delete_segment:
            case HEADER_f_open:
            case HEADER_f_close:
            case HEADER_f_seek:
            case HEADER_f_truncate:
            case HEADER_f_read:
            case HEADER_f_write:

             pcb_set_program_counter(pcbAActualizar, programCounterActualizado);

             pcb_set_registro_ax_cpu(pcbAActualizar, registroAxActualizado);
             pcb_set_registro_bx_cpu(pcbAActualizar, registroAxActualizado);
             pcb_set_registro_cx_cpu(pcbAActualizar, registroAxActualizado);
             pcb_set_registro_dx_cpu(pcbAActualizar, registroAxActualizado);

             break;

             default: break;

        }
    
    buffer_destroy(bufferPcb);
    return pcbAActualizar;

    }  
    else{
        
        log_error(kernelLogger, "Error al recibir PCB de CPU");
        exit(EXIT_FAILURE);
    }

}