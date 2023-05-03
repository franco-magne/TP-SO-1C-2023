#include "../include/consola-parser.h"

bool consola_parser_parse_instructions(t_buffer *buffer, const char *pathInstrucciones, t_log *consolaLogger) 
{
    bool parseSuccess = false;
    uint32_t op1 = -1;
    uint32_t op2 = -1;
    char *instruccion, *auxInstruccion;

    char** vectorStringsInstruccion;
    char *identificadorInstruccion, *arg1, *arg2, *arg3;
    t_registro auxRegistro1, auxRegistro2;
    
    //Implementar en static utils funcion abrir_archivo con manejo de errores
    FILE *archivoInstrucciones = abrir_archivo(pathInstrucciones, "r", consolaLogger);
    if(archivoInstrucciones == NULL) return parseSuccess;
    
    for (;;) {
        
        instruccion = malloc(MAX_LENGTH_INSTRUCTION);

        if(fgets(instruccion, MAX_LENGTH_INSTRUCTION, archivoInstrucciones) != NULL){ 
            
            if(string_equals_ignore_case(instruccion, "\n")){

                free(instruccion);
                continue;
            }
            
            auxInstruccion = string_duplicate(instruccion); 
            string_trim(&auxInstruccion);
            vectorStringsInstruccion = string_split(auxInstruccion, " "); 
            identificadorInstruccion = string_duplicate(vectorStringsInstruccion[0]); 
            string_trim(&identificadorInstruccion); 
           
            if(es_instruccion_valida(identificadorInstruccion)){ 
                 
                if(es_instruccion_con_dos_argumentos(identificadorInstruccion)){
                    
    
      
                            /*
                            
                            F_READ, F_WRITE: 3 parámetros
                            SET, MOV_IN, MOV_OUT, F_TRUNCATE, F_SEEK, CREATE_SEGMENT: 2 parámetros.
                            I/O, WAIT, SIGNAL, F_OPEN, F_CLOSE, DELETE_SEGMENT: 1 parámetro. 
                            EXIT, YIELD: 0 parámetros.

                            */
                    switch (obtener_tipo_instruccion(identificadorInstruccion))
                    {
                        case INSTRUCCION_SET:
                        case INSTRUCCION_MOV_IN:
                            arg1 = string_duplicate(vectorStringsInstruccion[1]);
                            string_trim(&arg1);
                            auxRegistro1 = obtener_registro(arg1);
                            arg2 = string_duplicate(vectorStringsInstruccion[2]);
                            string_trim(&arg2);
                            op2 = (uint32_t) atoi(arg2);
                            consola_serializer_pack_two_args(buffer, obtener_tipo_instruccion(identificadorInstruccion), (void*)&auxRegistro1, (void*)&op2);
                            break;
                        
                        case INSTRUCCION_F_TRUNCATE:
                        case INSTRUCCION_F_SEEK:
                            // F_TRUNCATE STRING INT
                            arg1 = string_duplicate(vectorStringsInstruccion[1]);
                            string_trim(&arg1);
                            arg2 = string_duplicate(vectorStringsInstruccion[2]);
                            string_trim(&arg2);
                            op2 = (uint32_t) atoi(arg2);
                            consola_serializer_pack_two_args(buffer, obtener_tipo_instruccion(identificadorInstruccion), (void*)&arg1, (void*)&op2);
                            break;

                        case INSTRUCCION_MOV_OUT:
                
                            arg1 = string_duplicate(vectorStringsInstruccion[1]);
                            string_trim(&arg1);
                            op1 = (uint32_t) atoi(arg1);
                            arg2 = string_duplicate(vectorStringsInstruccion[2]);
                            string_trim(&arg2);
                            auxRegistro2 = obtener_registro(arg2);
                            consola_serializer_pack_two_args(buffer, obtener_tipo_instruccion(identificadorInstruccion), (void*)&op1, (void*)&auxRegistro2);
                            break;
                        
                        case INSTRUCCION_IO:
                        case INSTRUCCION_DELETE_SEGMENT:
                            arg1 = string_duplicate(vectorStringsInstruccion[1]);
                            string_trim(&arg1);
                            op1 = (uint32_t) atoi(arg1);
                            consola_serializer_pack_one_integer_args(buffer, obtener_tipo_instruccion(identificadorInstruccion), &op2 );
                            break;
                        case INSTRUCCION_WAIT:
                        case INSTRUCCION_F_OPEN:
                        case INSTRUCCION_F_CLOSE:
                            arg1 = string_duplicate(vectorStringsInstruccion[1]);
                            string_trim(&arg1);
                            consola_serializer_pack_one_string_args(buffer, obtener_tipo_instruccion(identificadorInstruccion), &arg1 );
                            break;
                        case INSTRUCCION_CREATE_SEGMENT:
                            arg1 = string_duplicate(vectorStringsInstruccion[1]);
                            string_trim(&arg1);
                            op1 = (uint32_t) atoi(arg1);
                            arg2 = string_duplicate(vectorStringsInstruccion[2]);
                            string_trim(&arg2);
                            op2 = (uint32_t) atoi(arg2);
                            consola_serializer_pack_two_args(buffer, obtener_tipo_instruccion(identificadorInstruccion), (void*)&op1, (void*)&op2);
                            break;
                        case INSTRUCCION_F_WRITE:
                        case INSTRUCCION_F_READ:
                            arg1 = string_duplicate(vectorStringsInstruccion[1]);
                            string_trim(&arg1);
                            arg2 = string_duplicate(vectorStringsInstruccion[2]);
                            string_trim(&arg2);
                            op1 = (uint32_t) atoi(arg2);
                            arg3 = string_duplicate(vectorStringsInstruccion[3]);
                            string_trim(&arg3);
                            op2 = (uint32_t) atoi(arg3);
            
                            consola_serializer_pack_three_args(buffer, obtener_tipo_instruccion(identificadorInstruccion), &arg1, &op1, &op2);
                            
                            break;
         
                        default:
                            break;
                    }    
                    
                log_info(consolaLogger, "Se empaqueta instruccion: %s con operandos %s y %s", identificadorInstruccion, arg1, arg2);
                free(instruccion);
                free(auxInstruccion);
                //destruir_vector_strings(vectorStringsInstruccion);
                free(identificadorInstruccion);
                free(arg1);
                free(arg2);
                continue;
                    
                }
                else{

                    if(cantidad_argumentos(vectorStringsInstruccion) > 0){

                        log_error(consolaLogger, "La instruccion EXIT no debe contener ningun argumento. Uso: EXIT");
                        free(instruccion);
                        free(auxInstruccion);
                        //destruir_vector_strings(vectorStringsInstruccion);
                        free(identificadorInstruccion);
                        parseSuccess = false;
                        break;
                    }

                        consola_serializer_pack_no_args(buffer, obtener_tipo_instruccion(identificadorInstruccion));
                        log_info(consolaLogger, "Se empaqueta la instruccion: %s", instruccion);
                        free(instruccion);
                        free(auxInstruccion);
                        //destruir_vector_strings(vectorStringsInstruccion);
                        free(identificadorInstruccion);
                        parseSuccess = true;
                        break;
                }
            }
            else{
                
                log_error(consolaLogger, "Instruccion %s no reconocida", identificadorInstruccion);
                free(instruccion);
                free(auxInstruccion);
                //destruir_vector_strings(vectorStringsInstruccion);
                free(identificadorInstruccion);
                parseSuccess = false;
                break;
            }
        }
        else{

            log_error(consolaLogger, "Error al leer el archivo %s de instrucciones: %s", pathInstrucciones, strerror(errno));
            free(instruccion);
            parseSuccess = false;
            break;
        }

    }

    fclose(archivoInstrucciones);
    return parseSuccess;
}

bool es_instruccion_con_tres_argumentos(char* instruccion)
{
    return !strcmp(instruccion, "F_READ") 
        || !strcmp(instruccion, "F_WRITE");
}

bool es_instruccion_con_dos_argumentos(char* instruccion)
{
    return !strcmp(instruccion, "SET") 
        || !strcmp(instruccion, "MOV_IN")
        || !strcmp(instruccion, "MOV_OUT")
        || !strcmp(instruccion, "F_TRUNCATE")
        || !strcmp(instruccion, "F_SEEK")
        || !strcmp(instruccion, "CREATE_SEGMENT");
}

bool es_instruccion_valida(char* instruccion)
{
    return es_instruccion_con_dos_argumentos(instruccion)
        || !strcmp(instruccion, "EXIT");
}

t_tipo_instruccion obtener_tipo_instruccion(char* instruccion)
{
    if(!strcmp(instruccion, "SET")) return INSTRUCCION_SET;
    else if(!strcmp(instruccion, "MOV_OUT")) return INSTRUCCION_MOV_OUT;
    else if(!strcmp(instruccion, "WAIT")) return INSTRUCCION_WAIT;
    else if(!strcmp(instruccion, "IO")) return INSTRUCCION_IO;
    else if(!strcmp(instruccion, "SIGNAL")) return INSTRUCCION_SIGNAL;
    else if(!strcmp(instruccion, "MOV_IN")) return INSTRUCCION_MOV_IN;
    else if(!strcmp(instruccion, "F_OPEN")) return INSTRUCCION_F_OPEN;
    else if(!strcmp(instruccion, "YIELD")) return INSTRUCCION_YIELD;
    else if(!strcmp(instruccion, "F_TRUNCATE")) return INSTRUCCION_F_TRUNCATE;
    else if(!strcmp(instruccion, "F_SEEK")) return INSTRUCCION_F_SEEK;
    else if(!strcmp(instruccion, "CREATE_SEGMENT")) return INSTRUCCION_CREATE_SEGMENT;
    else if(!strcmp(instruccion, "F_WRITE")) return INSTRUCCION_F_WRITE;
    else if(!strcmp(instruccion, "F_READ")) return INSTRUCCION_F_READ;
    else if(!strcmp(instruccion, "DELETE_SEGMENT")) return INSTRUCCION_DELETE_SEGMENT;
    else if(!strcmp(instruccion, "F_CLOSE")) return INSTRUCCION_F_CLOSE;
    else if(!strcmp(instruccion, "EXIT")) return INSTRUCCION_EXIT;
    else return INSTRUCCION_error;
}

t_registro obtener_registro(char* registro)
{
    if(!strcmp(registro, "AX")) return REGISTRO_ax;
    else if(!strcmp(registro, "BX")) return REGISTRO_bx;
    else if(!strcmp(registro, "CX")) return REGISTRO_cx;
    else return REGISTRO_dx;
}


int cantidad_argumentos(char** vectorInstruccion)
{
    int cantidadArgumentos = 0;

    for(int i = 0; vectorInstruccion[i] != NULL; i++){

        cantidadArgumentos++;
    }

    return cantidadArgumentos - 1;
}