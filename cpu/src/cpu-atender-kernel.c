#include <../include/cpu-atender-kernel.h>





static bool hayInterrupcion;
static int pidProcesoEnExec;
static pthread_mutex_t mutexInterrupcion;

//utils functions 
static char* t_registro_to_char(t_registro registro)
{
    if(registro == REGISTRO_ax) {

        return "AX";
    }
    else if(registro == REGISTRO_bx) {

        return "BX";
    }
    else if(registro == REGISTRO_cx) {
        
        return "CX";
    }
    else if(registro == REGISTRO_dx) {
        
        return "DX";
    }
    else {

        return "NULL";
    }
}



//Dispatch module

//ciclo de instruccion 
static t_instruccion* cpu_fetch_instruction(t_cpu_pcb* pcb) 
{
    t_list* instructionsList = cpu_pcb_get_instrucciones(pcb);
    uint32_t programCounter = cpu_pcb_get_program_counter(pcb);
    
    t_instruccion* nextInstruction = list_get(instructionsList, programCounter);
    log_info(cpuLogger, "FETCH INSTRUCTION: PCB <ID %d>", cpu_pcb_get_pid(pcb));
    
    return nextInstruction;
}

static bool cpu_decode_instruction(uint32_t pid, t_instruccion* instruction) 
{
    //char* instruccionString = instruccion_to_string(instruction);
    log_info(cpuLogger, "DECODE INSTRUCTION: PCB <ID %d> Decoded Instruction: X", pid);
    //free(instruccionString);
    
    //return instruccion_get_tipo_instruccion(instruction) == INSTRUCCION_MOV_IN;
    return false;
}

static uint32_t cpu_fetch_operands(t_instruccion* nextInstruction, t_cpu_pcb* pcb) 
{
    /*
    uint32_t direccionLogicaOrigen = instruccion_get_operando2(nextInstruction);
    
    //printf("CPU tabla página primer nivel en CPU_fetch_operands: %d\n", cpu_pcb_get_array_tabla_paginas(pcb));
    uint32_t fetchedValue = cpu_leer_en_memoria(tlb, cpu_config_get_socket_memoria(cpuConfig), direccionLogicaOrigen, cpu_pcb_get_tabla_pagina_primer_nivel(pcb));
    log_info(cpuDevLogger, "FETCH OPERANDS: PCB <ID %d> COPY <DL Destino: %d> <DL Origen: %d> => Fetched Value: %d", cpu_pcb_get_pid(pcb), instruccion_get_operando1(nextInstruction), direccionLogicaOrigen, fetchedValue);
    return fetchedValue;
    */

   return 0;
}


static char*  get_registro_segun_tipo(t_registro tipoRegistro, t_cpu_pcb* pcb)
{
    switch (tipoRegistro)
    {
        case REGISTRO_ax:
            
            return cpu_pcb_get_registro_ax(pcb);
            break;

        case REGISTRO_bx:
            
            return cpu_pcb_get_registro_bx(pcb);
            break;

        case REGISTRO_cx:
            
            return cpu_pcb_get_registro_cx(pcb);
            break;

        case REGISTRO_dx:
            
            return cpu_pcb_get_registro_dx(pcb);
            break;

        default:
            return 0;
            break;
    }
}

static void set_registro_segun_tipo(t_registro tipoRegistro, char* valorASetear, t_cpu_pcb* pcb)
{
    switch (tipoRegistro)
    {
        case REGISTRO_ax:
            
            cpu_pcb_set_registro_ax(pcb, valorASetear);
            break;

        case REGISTRO_bx:
            
            cpu_pcb_set_registro_bx(pcb, valorASetear);
            break;

        case REGISTRO_cx:
            
            cpu_pcb_set_registro_cx(pcb, valorASetear);
            break;

        case REGISTRO_dx:
            
            cpu_pcb_set_registro_dx(pcb, valorASetear);
            break;

        default:
            break;
    }

    log_info(cpuLogger, "Registro %s seteado con valor: %s", t_registro_to_char(tipoRegistro), valorASetear ); // ver el tema de valor a setear para utilizar la funcion get_registro_segun_tipo que no la estamos usando
}

static bool cpu_exec_instruction(t_cpu_pcb* pcb, t_tipo_instruccion tipoInstruccion, void* operando1, void* operando2) 
{
    uint32_t programCounterActualizado = cpu_pcb_get_program_counter(pcb) + 1;
    bool shouldStopExec = false;
    //bool shouldWrite = false;
    //char* logMsg = NULL;
    
    if (tipoInstruccion == INSTRUCCION_SET) {
        
        t_registro registroASetear = *((t_registro*) operando1);
        char* valorASetear = string_duplicate((char*) operando2);;
        uint32_t retardoInstruccion = cpu_config_get_retardo_instruccion(cpuConfig);

        
        log_info(cpuLogger, "PID: <%d> - Ejecutando: <SET> - <%s> - <%s>", cpu_pcb_get_pid(pcb), t_registro_to_char(registroASetear), valorASetear);
        
        intervalo_de_pausa(retardoInstruccion);
        
        set_registro_segun_tipo(registroASetear, valorASetear, pcb);

        cpu_pcb_set_program_counter(pcb, programCounterActualizado);
    } else if (tipoInstruccion == INSTRUCCION_EXIT) {
        
        
        log_info(cpuLogger, "PID: <%d> - Ejecutando: <EXIT> - <NULL> - <NULL>", cpu_pcb_get_pid(pcb));

        uint32_t pid = cpu_pcb_get_pid(pcb);
      //  uint32_t* arrayTablaPaginasActualizado = cpu_pcb_get_array_tabla_paginas(pcb);
        t_registros_cpu* registrosCpuActualizado = cpu_pcb_get_registros(pcb);
        
        t_buffer* bufferExit = buffer_create();

         //Empaqueto pid
        buffer_pack(bufferExit, &pid, sizeof(pid));

        //Empaqueto pc
        buffer_pack(bufferExit, &programCounterActualizado, sizeof(programCounterActualizado));

        //Empaquetamos registros
        buffer_pack(bufferExit, &registrosCpuActualizado->registroAx, sizeof(registrosCpuActualizado->registroAx));
        buffer_pack(bufferExit, &registrosCpuActualizado->registroBx, sizeof(registrosCpuActualizado->registroBx));
        buffer_pack(bufferExit, &registrosCpuActualizado->registroCx, sizeof(registrosCpuActualizado->registroCx));
        buffer_pack(bufferExit, &registrosCpuActualizado->registroDx, sizeof(registrosCpuActualizado->registroDx));
        
        stream_send_buffer(cpu_config_get_socket_dispatch(cpuConfig), HEADER_proceso_terminado, bufferExit);
        buffer_destroy(bufferExit);
        
        shouldStopExec = true;
        cpu_pcb_set_program_counter(pcb, programCounterActualizado);
    }

    return shouldStopExec;
}

static bool cpu_ejecutar_ciclos_de_instruccion(t_cpu_pcb* pcb) 
{
    bool shouldStopExec = false;
    uint32_t operando1 = 0;
    uint32_t operando2 = NULL;
    t_registro registro1 = REGISTRO_null;
    t_registro registro2 = REGISTRO_null;
    char* dispositivo = NULL;

    t_instruccion* nextInstruction = cpu_fetch_instruction(pcb);
    bool shouldFetchOperands = cpu_decode_instruction(cpu_pcb_get_pid(pcb), nextInstruction);
    t_tipo_instruccion tipoInstruccion = instruccion_get_tipo_instruccion(nextInstruction);
    
    switch (tipoInstruccion)
    {   
        case INSTRUCCION_SET:
            
            registro1 = instruccion_get_registro1(nextInstruction);

            if (shouldFetchOperands) {
                
                dispositivo = cpu_fetch_operands(nextInstruction, pcb);
                
            }
            else {

                dispositivo = instruccion_get_dispositivo(nextInstruction);
                
            }

            shouldStopExec = cpu_exec_instruction(pcb, tipoInstruccion, (void*) &registro1, (void*) dispositivo);
            break;
        
      
        case INSTRUCCION_EXIT:

            shouldStopExec = cpu_exec_instruction(pcb, tipoInstruccion, NULL, NULL);
            break;
        
        default:
            break;
    }

    return shouldStopExec;
}

static void dispatch_peticiones_de_kernel() 
{
    
    uint32_t pidRecibido = 0;
    uint32_t programCounter = 0;
   // uint32_t *arrayDeSegmentos = NULL;
    t_registros_cpu* registrosCpu = NULL;
   
    
    for (;;) {
        
        uint8_t kernelResponse = stream_recv_header(cpu_config_get_socket_dispatch(cpuConfig));
               


        t_buffer* bufferPcb = NULL;
        t_cpu_pcb* pcb = NULL;


        if (kernelResponse == HEADER_pcb_a_ejecutar) {
            
            log_info(cpuLogger, "TEST"); 

            bufferPcb = buffer_create();
            stream_recv_buffer(cpu_config_get_socket_dispatch(cpuConfig), bufferPcb);
            
            //empezamos a desempaquetar 
            //Desempaquetamos pid pcb
            buffer_unpack(bufferPcb, &pidRecibido, sizeof(pidRecibido));
            //Desempaquetamos pc pcb
            buffer_unpack(bufferPcb, &programCounter, sizeof(programCounter));


            registrosCpu = registros_cpu_create();
            //Desempaquetamos registros cpu
            buffer_unpack(bufferPcb, &registrosCpu->registroAx, sizeof(registrosCpu->registroAx));
            buffer_unpack(bufferPcb, &registrosCpu->registroBx, sizeof(registrosCpu->registroBx));
            buffer_unpack(bufferPcb, &registrosCpu->registroCx, sizeof(registrosCpu->registroCx));
            buffer_unpack(bufferPcb, &registrosCpu->registroDx, sizeof(registrosCpu->registroDx));
            
            /*Desempaquetamos todo los datos que nos trae socket dispatch y lo guardamos*/

            buffer_destroy(bufferPcb);
            
            // Aca hace referencia a que un proceso puede estar ejecutando entonces no lo manda a ejecutar de nuevo
            if (pidRecibido != pidProcesoEnExec) {
                pidProcesoEnExec = pidRecibido;
            }
            
            pcb = cpu_pcb_create(pidRecibido, programCounter, registrosCpu); 

            kernelResponse = stream_recv_header(cpu_config_get_socket_dispatch(cpuConfig));
            
            // Aca guardamos la lista de instrucciones
            if (kernelResponse == HEADER_lista_instrucciones) {
                
                t_buffer* bufferInstrucciones = buffer_create();
                stream_recv_buffer(cpu_config_get_socket_dispatch(cpuConfig), bufferInstrucciones);
                
                t_list* listaInstrucciones = lista_de_instrucciones_buffer(bufferInstrucciones, cpuLogger);  // Agregar a las commons?
                cpu_pcb_set_instrucciones(pcb, listaInstrucciones);
                
                buffer_destroy(bufferInstrucciones);
            } 
            else {

                log_error(cpuLogger, "Error al intentar recibir las instrucciones de Kernel");  //DevLogger
                exit(EXIT_FAILURE);
            }
            
            bool shouldStopExec = false;
            
            while (!shouldStopExec)  {
                
                shouldStopExec = cpu_ejecutar_ciclos_de_instruccion(pcb);
            }
            
            //cpu_pcb_destroy(pcb);
        }
        else {
            
            log_error(cpuLogger, "Error al intentar recibir el PCB de Kernel");
            exit(EXIT_FAILURE);
        }


        pidRecibido = 0;
        programCounter = 0;
        //arrayDeSegmentos = NULL;
        registrosCpu = NULL;
    }
}


void atender_peticiones_de_kernel() 
{
    
    //pthread_t interruptTh; 
    //pthread_create(&interruptTh, NULL, (void*)dispatch_peticiones_de_kernel, NULL);
    //pthread_detach(interruptTh);
    dispatch_peticiones_de_kernel();
    log_info(cpuLogger, "Hilos de atención creados. Listo para atender peticiones de Kernel");
    
}
