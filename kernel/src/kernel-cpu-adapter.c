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


    //Empaquetamos los registros
    buffer_pack(bufferPcbAEjecutar, &registrosCpu->registroAx, sizeof(uint32_t));
    buffer_pack(bufferPcbAEjecutar, &registrosCpu->registroBx, sizeof(uint32_t));
    buffer_pack(bufferPcbAEjecutar, &registrosCpu->registroCx, sizeof(uint32_t));
    buffer_pack(bufferPcbAEjecutar, &registrosCpu->registroDx, sizeof(uint32_t));

    //stream_send_empty_buffer(kernel_config_get_socket_dispatch_cpu(kernelConfig), header);
    stream_send_buffer(kernel_config_get_socket_dispatch_cpu(kernelConfig), header, bufferPcbAEjecutar);
    stream_send_buffer(kernel_config_get_socket_dispatch_cpu(kernelConfig), HEADER_lista_instrucciones, pcb_get_instrucciones_buffer(pcbAEnviar));

    buffer_destroy(bufferPcbAEjecutar);
}

t_pcb* cpu_adapter_recibir_pcb_actualizado_de_cpu(t_pcb* pcbAActualizar, uint8_t cpuResponse, t_kernel_config* kernelConfig, t_log* kernelLogger)
{
    uint32_t pidRecibido = 0;
    uint32_t programCounterActualizado = 0;
    //uint32_t* tablaPagsActualizada = 0;
    uint32_t registroAxActualizado = 0, registroBxActualizado = 0, registroCxActualizado = 0, registroDxActualizado = 0;
    //char* dispositivoIoEnUso = NULL;
    uint32_t cantidadUnidadesTiemposIo = 0;
    //t_registro registroEnUsoIo = REGISTRO_null;

    t_buffer* bufferPcb = buffer_create();

    stream_recv_buffer(kernel_config_get_socket_dispatch_cpu(kernelConfig), bufferPcb);
    buffer_unpack(bufferPcb, &pidRecibido, sizeof(pidRecibido));
    buffer_unpack(bufferPcb, &programCounterActualizado, sizeof(programCounterActualizado));

    //desempaquetar tabla paginas
    //uint32_t tamanioArrayTablaPaginas = tamanio_array_enteros(pcbAActualizar->arrayTablaPaginas) * sizeof(uint32_t);
    //tablaPagsActualizada = malloc(tamanioArrayTablaPaginas);
    //buffer_unpack(bufferPcb, tablaPagsActualizada, tamanioArrayTablaPaginas );

    //desempaquetar regs 
    buffer_unpack(bufferPcb, &registroAxActualizado , sizeof(uint32_t));
    buffer_unpack(bufferPcb, &registroBxActualizado , sizeof(uint32_t));
    buffer_unpack(bufferPcb, &registroCxActualizado , sizeof(uint32_t));
    buffer_unpack(bufferPcb, &registroDxActualizado , sizeof(uint32_t));

   if (pidRecibido == pcb_get_pid(pcbAActualizar)) {
        /* 
        if (cpuResponse == HEADER_proceso_bloqueado) {
            
    
        pcb_set_program_counter(pcbAActualizar, programCounterActualizado);

        pcb_set_registro_ax_cpu(pcbAActualizar, registroAxActualizado);
        pcb_set_registro_bx_cpu(pcbAActualizar, registroAxActualizado);
        pcb_set_registro_cx_cpu(pcbAActualizar, registroAxActualizado);
        pcb_set_registro_dx_cpu(pcbAActualizar, registroAxActualizado);
        } 
        else {
        
        log_error(kernelLogger, "Error al recibir PCB de CPU");
        exit(EXIT_FAILURE);
        }
        */ 
    buffer_destroy(bufferPcb);
    
    return pcbAActualizar;
    }

}