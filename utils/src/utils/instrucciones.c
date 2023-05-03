#include "instrucciones.h"

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


char* instruccion_to_string(t_tipo_instruccion chequea, t_info_instruccion* self) 
{
  
    uint32_t operando1 = self->operando1;
    uint32_t operando2 = self->operando2;
    t_registro registro1 = self->registro1;
    t_registro registro2 = self->registro2;
    char* dispotisivo = self->dispositivo; 
    
    return 
             chequea == INSTRUCCION_CREATE_SEGMENT  ? string_from_format("CREATE_SEGMENT %i %i", operando1, operando2)
           : chequea == INSTRUCCION_MOV_IN    ? string_from_format("MOV_IN %s %d",t_registro_to_char(registro1) , operando2)
           : chequea == INSTRUCCION_SET   ? string_from_format("SET %s %s",t_registro_to_char(registro1) ,dispotisivo)
           : chequea == INSTRUCCION_MOV_OUT  ? string_from_format("MOV_OUT %d %s", operando1, t_registro_to_char(registro2))
           : chequea == INSTRUCCION_EXIT ? string_from_format("EXIT")
           : string_from_format("UNKNOWN");
}



t_instruccion* instruccion_create(t_tipo_instruccion tipoInstruccion, t_info_instruccion* infoInstruccion) {
    t_instruccion* self = malloc(sizeof(*self));
    self->tipoInstruccion = tipoInstruccion;
    self->operando1 = infoInstruccion->operando1;
    self->operando2 = infoInstruccion->operando2;
    self->dispositivo = infoInstruccion->dispositivo;
    self->registro1 = infoInstruccion->registro1;
    self->registro2 = infoInstruccion->registro2;
    //self->toString = NULL;//instruccion_to_string(self);
    return self;
}

t_list* lista_de_instrucciones_buffer(t_buffer* bufferConInstrucciones, t_log * logger){
    t_list* instrucciones = list_create();
    t_tipo_instruccion identificadorInstruccion = -1;
    bool isExit = false;
    while (!isExit) {
        buffer_unpack(bufferConInstrucciones, &identificadorInstruccion, sizeof(identificadorInstruccion));
        
        t_info_instruccion* infoInstruccion = malloc(sizeof(t_info_instruccion));
        infoInstruccion->operando1 = -1;
        infoInstruccion->operando2 = -1;
        infoInstruccion->registro1= REGISTRO_null;
        infoInstruccion->registro2= REGISTRO_null;
        infoInstruccion->dispositivo = NULL; 
 
        /*
        
        F_READ, F_WRITE: 3 parámetros
        SET, MOV_IN, MOV_OUT, F_TRUNCATE, F_SEEK, CREATE_SEGMENT: 2 parámetros.
        I/O, WAIT, SIGNAL, F_OPEN, F_CLOSE, DELETE_SEGMENT: 1 parámetro. 
        EXIT, YIELD: 0 parámetros.

        */

        switch (identificadorInstruccion) {
            case INSTRUCCION_YIELD:
                break;
/////////////////////////////////////// 1 PARAMETROS ///////////////////////////////////
            case INSTRUCCION_IO:
                buffer_unpack(bufferConInstrucciones, &infoInstruccion->operando1 , sizeof(infoInstruccion->operando1) );
                break;
            case INSTRUCCION_WAIT:
            case INSTRUCCION_SIGNAL:
                buffer_unpack(bufferConInstrucciones, &infoInstruccion->dispositivo , sizeof(infoInstruccion->dispositivo) );
                break;
            case INSTRUCCION_F_OPEN:
            case INSTRUCCION_F_CLOSE:
                buffer_unpack(bufferConInstrucciones, &infoInstruccion->dispositivo , sizeof(infoInstruccion->dispositivo) );
                break;
            case INSTRUCCION_DELETE_SEGMENT:
                buffer_unpack(bufferConInstrucciones, &infoInstruccion->operando1 , sizeof(infoInstruccion->operando1));
                break;
/////////////////////////////////////// 2 PARAMETROS ///////////////////////////////////
            case INSTRUCCION_SET:
                buffer_unpack(bufferConInstrucciones, &infoInstruccion->registro1 , sizeof(infoInstruccion->registro1) );
                infoInstruccion->dispositivo = buffer_unpack_string(bufferConInstrucciones);
                break;

            case INSTRUCCION_MOV_IN:
    
                buffer_unpack(bufferConInstrucciones, &infoInstruccion->registro1 , sizeof(infoInstruccion->registro1) );
                buffer_unpack(bufferConInstrucciones, &infoInstruccion->operando2 , sizeof(infoInstruccion->operando2 ) );
                break;
            case INSTRUCCION_MOV_OUT:
                buffer_unpack(bufferConInstrucciones, &infoInstruccion->operando1 , sizeof(infoInstruccion->operando1) );
                buffer_unpack(bufferConInstrucciones, &infoInstruccion->registro2 , sizeof(infoInstruccion->registro2) );
                break;
            case INSTRUCCION_F_TRUNCATE:
                buffer_unpack(bufferConInstrucciones, &infoInstruccion->dispositivo , sizeof(infoInstruccion->dispositivo) );
                buffer_unpack(bufferConInstrucciones, &infoInstruccion->operando1 , sizeof(infoInstruccion->operando1) );
                break;
            case INSTRUCCION_F_SEEK:
                buffer_unpack(bufferConInstrucciones, &infoInstruccion->dispositivo , sizeof(infoInstruccion->dispositivo) );
                buffer_unpack(bufferConInstrucciones, &infoInstruccion->operando1 , sizeof(infoInstruccion->operando1) );
                break;
            case INSTRUCCION_CREATE_SEGMENT:
                buffer_unpack(bufferConInstrucciones, &infoInstruccion->operando1 , sizeof(infoInstruccion->operando1) );
                buffer_unpack(bufferConInstrucciones, &infoInstruccion->operando2 , sizeof(infoInstruccion->operando2) );
                break;
/////////////////////////////////////// 3 PARAMETROS ///////////////////////////////////
            case INSTRUCCION_F_READ:
            case INSTRUCCION_F_WRITE:
                buffer_unpack(bufferConInstrucciones, &infoInstruccion->dispositivo , sizeof(infoInstruccion->dispositivo) );
                buffer_unpack(bufferConInstrucciones, &infoInstruccion->operando1 , sizeof(infoInstruccion->operando1) );
                buffer_unpack(bufferConInstrucciones, &infoInstruccion->operando2 , sizeof(infoInstruccion->operando2) );
                break;       
            case INSTRUCCION_EXIT:
                isExit = true;
                break;
            default:
                log_error(logger, "Error al intentar desempaquetar una instrucción");
                exit(-1);
        }
        t_instruccion* instruccionActual = instruccion_create(identificadorInstruccion, infoInstruccion);
        list_add(instrucciones, instruccionActual);
        log_info(logger, "Se desempaqueta la instruccion %s", instruccion_to_string(identificadorInstruccion,infoInstruccion));
    }
    log_info(logger, "Se desempaquetan %d instrucciones", list_size(instrucciones));
    return instrucciones;
}





