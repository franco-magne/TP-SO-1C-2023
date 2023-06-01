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
    if(registro == REGISTRO_eax) {

        return "EAX";
    }
    else if(registro == REGISTRO_ebx) {

        return "EBX";
    }
    else if(registro == REGISTRO_ecx) {
        
        return "ECX";
    }
    else if(registro == REGISTRO_edx) {
        
        return "EDX";
    }
    if(registro == REGISTRO_rax) {

        return "RAX";
    }
    else if(registro == REGISTRO_rbx) {

        return "RBX";
    }
    else if(registro == REGISTRO_rcx) {
        
        return "RCX";
    }
    else if(registro == REGISTRO_rdx) {
        
        return "RDX";
    }
    else {

        return "NULL";
    }
}

void empaquetar_instruccion(uint32_t pid, uint32_t programCounterActualizado, t_registros_cpu * registrosCpuActualizado, uint8_t header, recurso* recursos){

        // esto es para las instrucciones bloqueantes IO y posibles bloqueantes wait y signal
        uint32_t  unidadesDeTrabajo = cpu_get_recurso_IO(recursos);
        char* recurso_utilizado = cpu_get_recurso_sem(recursos);

        t_buffer* buffer = buffer_create();

         //Empaqueto pid
        buffer_pack(buffer, &pid, sizeof(pid));

        //Empaqueto pc
        buffer_pack(buffer, &programCounterActualizado, sizeof(programCounterActualizado));

        //Empaquetamos registros
        buffer_pack(buffer, &registrosCpuActualizado->registroAx, sizeof(registrosCpuActualizado->registroAx));
        buffer_pack(buffer, &registrosCpuActualizado->registroBx, sizeof(registrosCpuActualizado->registroBx));
        buffer_pack(buffer, &registrosCpuActualizado->registroCx, sizeof(registrosCpuActualizado->registroCx));
        buffer_pack(buffer, &registrosCpuActualizado->registroDx, sizeof(registrosCpuActualizado->registroDx));
        
        if(header == HEADER_proceso_bloqueado) buffer_pack(buffer, &unidadesDeTrabajo, sizeof(unidadesDeTrabajo));
        
        if(header == HEADER_proceso_pedir_recurso  || header == HEADER_proceso_devolver_recurso) buffer_pack(buffer, &recurso_utilizado, sizeof(recurso_utilizado)) ;

        stream_send_buffer(cpu_config_get_socket_dispatch(cpuConfig), header, buffer);
        buffer_destroy(buffer);
    
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
        
        case REGISTRO_eax:
            cpu_pcb_set_registro_eax(pcb,valorASetear);
            break;

        case REGISTRO_ebx:
            cpu_pcb_set_registro_ebx(pcb,valorASetear);
            break;

        case REGISTRO_ecx:
            cpu_pcb_set_registro_ecx(pcb,valorASetear);
            break;
        
        case REGISTRO_edx:
            cpu_pcb_set_registro_edx(pcb,valorASetear);
            break;
        
        case REGISTRO_rax:
            cpu_pcb_set_registro_rax(pcb,valorASetear);
            break;

        case REGISTRO_rbx:
            cpu_pcb_set_registro_rbx(pcb,valorASetear);
            break;

        case REGISTRO_rcx:
            cpu_pcb_set_registro_rcx(pcb,valorASetear);
            break;
        
        case REGISTRO_rdx:
            cpu_pcb_set_registro_rdx(pcb,valorASetear);
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
        char* valorASetear = string_duplicate((char*) operando2);
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
        
        empaquetar_instruccion(pid, programCounterActualizado,registrosCpuActualizado,HEADER_proceso_terminado, NULL);
        /*t_buffer* buffer = buffer_create();

         //Empaqueto pid
        buffer_pack(buffer, &pid, sizeof(pid));

        //Empaqueto pc
        buffer_pack(buffer, &programCounterActualizado, sizeof(programCounterActualizado));

        //Empaquetamos registros
        buffer_pack(buffer, &registrosCpuActualizado->registroAx, sizeof(registrosCpuActualizado->registroAx));
        buffer_pack(buffer, &registrosCpuActualizado->registroBx, sizeof(registrosCpuActualizado->registroBx));
        buffer_pack(buffer, &registrosCpuActualizado->registroCx, sizeof(registrosCpuActualizado->registroCx));
        buffer_pack(buffer, &registrosCpuActualizado->registroDx, sizeof(registrosCpuActualizado->registroDx));
        
        stream_send_buffer(cpu_config_get_socket_dispatch(cpuConfig), HEADER_proceso_terminado, buffer);
        buffer_destroy(buffer);
        */
        shouldStopExec = true;
        cpu_pcb_set_program_counter(pcb, programCounterActualizado);
    } else if (tipoInstruccion == INSTRUCCION_YIELD ) {
        
        
        log_info(cpuLogger, "PID: <%d> - Ejecutando: <YIELD> - <NULL> - <NULL>", cpu_pcb_get_pid(pcb));

        uint32_t pid = cpu_pcb_get_pid(pcb);
      //  uint32_t* arrayTablaPaginasActualizado = cpu_pcb_get_array_tabla_paginas(pcb);
        t_registros_cpu* registrosCpuActualizado = cpu_pcb_get_registros(pcb);
        
        empaquetar_instruccion(pid, programCounterActualizado,registrosCpuActualizado,HEADER_proceso_desalojado, NULL);

        /*
        t_buffer* bufferYield = buffer_create();

         //Empaqueto pid
        buffer_pack(bufferYield, &pid, sizeof(pid));

        //Empaqueto pc
        buffer_pack(bufferYield, &programCounterActualizado, sizeof(programCounterActualizado));

        //Empaquetamos registros
        buffer_pack(bufferYield, &registrosCpuActualizado->registroAx, sizeof(registrosCpuActualizado->registroAx));
        buffer_pack(bufferYield, &registrosCpuActualizado->registroBx, sizeof(registrosCpuActualizado->registroBx));
        buffer_pack(bufferYield, &registrosCpuActualizado->registroCx, sizeof(registrosCpuActualizado->registroCx));
        buffer_pack(bufferYield, &registrosCpuActualizado->registroDx, sizeof(registrosCpuActualizado->registroDx));
        
        stream_send_buffer(cpu_config_get_socket_dispatch(cpuConfig), HEADER_proceso_desalojado, bufferYield);
        buffer_destroy(bufferYield);
        */
        shouldStopExec = true;
        
        cpu_pcb_set_program_counter(pcb, programCounterActualizado);

        
    } else if (tipoInstruccion == INSTRUCCION_IO ) {
        
        uint32_t retardoInstruccion = cpu_config_get_retardo_instruccion(cpuConfig);//PROVISORIO !!!!!!!!!
        uint32_t unidadesDeTrabajo = *((uint32_t*) operando1);

        recurso* recursoIO = malloc(sizeof(recurso*));
        cpu_set_recursoIO(recursoIO, unidadesDeTrabajo);
        
        log_info(cpuLogger, "PID: <%d> - Ejecutando: <IO> - <%d> - <NULL>", cpu_pcb_get_pid(pcb) , unidadesDeTrabajo);

        //intervalo_de_pausa(retardoInstruccion);
        
        uint32_t pid = cpu_pcb_get_pid(pcb);
        t_registros_cpu* registrosCpuActualizado = cpu_pcb_get_registros(pcb);
        
        empaquetar_instruccion(pid, programCounterActualizado,registrosCpuActualizado,HEADER_proceso_bloqueado, recursoIO);

        /*
        t_buffer* bufferIO = buffer_create();

        //Empaqueto pid
        buffer_pack(bufferIO, &pid, sizeof(pid));

        //Empaqueto TIEMPO BLOQUEO OPERANDO 1
        
        //Empaqueto pc
        buffer_pack(bufferIO, &programCounterActualizado, sizeof(programCounterActualizado));
        
        //Empaquetamos registros
        buffer_pack(bufferIO, &registrosCpuActualizado->registroAx, sizeof(registrosCpuActualizado->registroAx));
        buffer_pack(bufferIO, &registrosCpuActualizado->registroBx, sizeof(registrosCpuActualizado->registroBx));
        buffer_pack(bufferIO, &registrosCpuActualizado->registroCx, sizeof(registrosCpuActualizado->registroCx));
        buffer_pack(bufferIO, &registrosCpuActualizado->registroDx, sizeof(registrosCpuActualizado->registroDx));
        
        buffer_pack(bufferIO, &unidadesDeTrabajo, sizeof(unidadesDeTrabajo));

        stream_send_buffer(cpu_config_get_socket_dispatch(cpuConfig), HEADER_proceso_bloqueado, bufferIO);
        buffer_destroy(bufferIO);
        */
        shouldStopExec = true;
        cpu_pcb_set_tiempoIO(pcb,unidadesDeTrabajo);
        cpu_pcb_set_program_counter(pcb, programCounterActualizado);
        free(recursoIO);

    } else if (tipoInstruccion == INSTRUCCION_SIGNAL ) {
        
        uint32_t retardoInstruccion = cpu_config_get_retardo_instruccion(cpuConfig);//PROVISORIO !!!!!!!!!
        log_info(cpuLogger, "PID: <%d> - Ejecutando: <SIGNAL> - <NULL> - <NULL>", cpu_pcb_get_pid(pcb));

        intervalo_de_pausa(retardoInstruccion);
        cpu_pcb_set_program_counter(pcb, programCounterActualizado);
        //cpu_pcb_set_registros

    } else if (tipoInstruccion == INSTRUCCION_WAIT ) {
        
        uint32_t retardoInstruccion = cpu_config_get_retardo_instruccion(cpuConfig);//PROVISORIO !!!!!!!!!
        char* recurso1 = string_duplicate((char*) operando1);
        log_info(cpuLogger, "PID: <%d> - Ejecutando: <WAIT> - <%s> - <NULL>", cpu_pcb_get_pid(pcb), recurso1);
      
        recurso *recursoWait = malloc(sizeof(recurso*));
        cpu_set_recurso_sem(recursoWait, recurso1);


        intervalo_de_pausa(retardoInstruccion);// PROVISORIO !!!!!

        cpu_pcb_set_program_counter(pcb, programCounterActualizado);
        cpu_pcb_set_recurso_utilizar(pcb, recurso1);


    } else if (tipoInstruccion == INSTRUCCION_CREATE_SEGMENT ) {
    
        uint32_t retardoInstruccion = cpu_config_get_retardo_instruccion(cpuConfig);//PROVISORIO !!!!!!!!!
        log_info(cpuLogger, "PID: <%d> - Ejecutando: <CREATE_SEGMENT> - <NULL> - <NULL>", cpu_pcb_get_pid(pcb));

        intervalo_de_pausa(retardoInstruccion);
        cpu_pcb_set_program_counter(pcb, programCounterActualizado);

    } else if (tipoInstruccion == INSTRUCCION_DELETE_SEGMENT ) {
        
        uint32_t retardoInstruccion = cpu_config_get_retardo_instruccion(cpuConfig);//PROVISORIO !!!!!!!!!
        log_info(cpuLogger, "PID: <%d> - Ejecutando: <DELETE_SEGMENT> - <NULL> - <NULL>", cpu_pcb_get_pid(pcb));

        intervalo_de_pausa(retardoInstruccion);
        cpu_pcb_set_program_counter(pcb, programCounterActualizado);

    } else if (tipoInstruccion == INSTRUCCION_F_OPEN ) {
        
        uint32_t retardoInstruccion = cpu_config_get_retardo_instruccion(cpuConfig);//PROVISORIO !!!!!!!!!
        log_info(cpuLogger, "PID: <%d> - Ejecutando: <F_OPEN> - <NULL> - <NULL>", cpu_pcb_get_pid(pcb));

        intervalo_de_pausa(retardoInstruccion);
        cpu_pcb_set_program_counter(pcb, programCounterActualizado);

    } else if (tipoInstruccion == INSTRUCCION_F_CLOSE ) {
        
        uint32_t retardoInstruccion = cpu_config_get_retardo_instruccion(cpuConfig);//PROVISORIO !!!!!!!!!
        log_info(cpuLogger, "PID: <%d> - Ejecutando: <F_CLOSE> - <NULL> - <NULL>", cpu_pcb_get_pid(pcb));

        intervalo_de_pausa(retardoInstruccion);
        cpu_pcb_set_program_counter(pcb, programCounterActualizado);

    } else if (tipoInstruccion == INSTRUCCION_F_READ ) {
        
        uint32_t retardoInstruccion = cpu_config_get_retardo_instruccion(cpuConfig);//PROVISORIO !!!!!!!!!
        log_info(cpuLogger, "PID: <%d> - Ejecutando: <F_READ> - <NULL> - <NULL>", cpu_pcb_get_pid(pcb));

        intervalo_de_pausa(retardoInstruccion);
        cpu_pcb_set_program_counter(pcb, programCounterActualizado);

    } else if (tipoInstruccion == INSTRUCCION_F_SEEK ) {
        
        uint32_t retardoInstruccion = cpu_config_get_retardo_instruccion(cpuConfig);//PROVISORIO !!!!!!!!!
        log_info(cpuLogger, "PID: <%d> - Ejecutando: <F_SEEK> - <NULL> - <NULL>", cpu_pcb_get_pid(pcb));

        intervalo_de_pausa(retardoInstruccion);
        cpu_pcb_set_program_counter(pcb, programCounterActualizado);

    } else if (tipoInstruccion == INSTRUCCION_F_TRUNCATE ) {
        
        uint32_t retardoInstruccion = cpu_config_get_retardo_instruccion(cpuConfig);//PROVISORIO !!!!!!!!!
        log_info(cpuLogger, "PID: <%d> - Ejecutando: <F_TRUNCATE> - <NULL> - <NULL>", cpu_pcb_get_pid(pcb));

        intervalo_de_pausa(retardoInstruccion);
        cpu_pcb_set_program_counter(pcb, programCounterActualizado);

    } else if (tipoInstruccion == INSTRUCCION_F_READ ) {
        
        uint32_t retardoInstruccion = cpu_config_get_retardo_instruccion(cpuConfig);//PROVISORIO !!!!!!!!!
        log_info(cpuLogger, "PID: <%d> - Ejecutando: <F_READ> - <NULL> - <NULL>", cpu_pcb_get_pid(pcb));

        intervalo_de_pausa(retardoInstruccion);
        cpu_pcb_set_program_counter(pcb, programCounterActualizado);

    } else if (tipoInstruccion == INSTRUCCION_MOV_IN ) {
        
        uint32_t retardoInstruccion = cpu_config_get_retardo_instruccion(cpuConfig);//PROVISORIO !!!!!!!!!
        log_info(cpuLogger, "PID: <%d> - Ejecutando: <MOV_IN> - <NULL> - <NULL>", cpu_pcb_get_pid(pcb));

        intervalo_de_pausa(retardoInstruccion);
        cpu_pcb_set_program_counter(pcb, programCounterActualizado);

    } else if (tipoInstruccion == INSTRUCCION_MOV_OUT ) {
        
        uint32_t retardoInstruccion = cpu_config_get_retardo_instruccion(cpuConfig);//PROVISORIO !!!!!!!!!
        log_info(cpuLogger, "PID: <%d> - Ejecutando: <MOV_OUT> - <NULL> - <NULL>", cpu_pcb_get_pid(pcb));

        intervalo_de_pausa(retardoInstruccion);
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
        case INSTRUCCION_YIELD:
            shouldStopExec = cpu_exec_instruction(pcb, tipoInstruccion, NULL, NULL);
            break;
        case INSTRUCCION_IO:
            
            operando1 = instruccion_get_operando1(nextInstruction);
            shouldStopExec = cpu_exec_instruction(pcb, tipoInstruccion,(void*) &operando1, NULL);
            break;
        case INSTRUCCION_SIGNAL:
            shouldStopExec = cpu_exec_instruction(pcb, tipoInstruccion, NULL, NULL);
            break;
        case INSTRUCCION_WAIT:
            dispositivo = instruccion_get_dispositivo(nextInstruction);
            shouldStopExec = cpu_exec_instruction(pcb, tipoInstruccion, dispositivo, NULL);
            break;
        case INSTRUCCION_CREATE_SEGMENT:
            shouldStopExec = cpu_exec_instruction(pcb, tipoInstruccion, NULL, NULL);
            break;
        case INSTRUCCION_DELETE_SEGMENT:
            shouldStopExec = cpu_exec_instruction(pcb, tipoInstruccion, NULL, NULL);
            break;
        case INSTRUCCION_F_OPEN:
            shouldStopExec = cpu_exec_instruction(pcb, tipoInstruccion, NULL, NULL);
            break;
        case INSTRUCCION_F_CLOSE:
            shouldStopExec = cpu_exec_instruction(pcb, tipoInstruccion, NULL, NULL);
            break;
        case INSTRUCCION_F_READ:
            shouldStopExec = cpu_exec_instruction(pcb, tipoInstruccion, NULL, NULL);
            break;
        case INSTRUCCION_F_SEEK:
            shouldStopExec = cpu_exec_instruction(pcb, tipoInstruccion, NULL, NULL);
            break;
        case INSTRUCCION_F_TRUNCATE:
            shouldStopExec = cpu_exec_instruction(pcb, tipoInstruccion, NULL, NULL);
            break;
        case INSTRUCCION_F_WRITE:
            shouldStopExec = cpu_exec_instruction(pcb, tipoInstruccion, NULL, NULL);
            break;
        case INSTRUCCION_MOV_IN:
            shouldStopExec = cpu_exec_instruction(pcb, tipoInstruccion, NULL, NULL);
            break;
        case INSTRUCCION_MOV_OUT:
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
    dispatch_peticiones_de_kernel();
    log_info(cpuLogger, "Hilos de atención creados. Listo para atender peticiones de Kernel");
}
