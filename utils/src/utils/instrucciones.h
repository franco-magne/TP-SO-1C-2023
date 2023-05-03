#ifndef INTRUCCIONES_H
#define INTRUCCIONES_H

//////////////// INCLUDE BASICOS ///////////////
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
///////////////// INCLUDE COMMONS //////////////
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/string.h>

//////////////// INCLUDE UTILS ////////////////
#include "serializacion.h"


typedef enum {
    INSTRUCCION_F_READ,
    INSTRUCCION_F_WRITE,
    INSTRUCCION_SET,
    INSTRUCCION_MOV_IN,
    INSTRUCCION_MOV_OUT,
    INSTRUCCION_F_TRUNCATE,
    INSTRUCCION_F_SEEK,
    INSTRUCCION_CREATE_SEGMENT,
    INSTRUCCION_IO,
    INSTRUCCION_WAIT,
    INSTRUCCION_SIGNAL,
    INSTRUCCION_F_OPEN,
    INSTRUCCION_F_CLOSE,
    INSTRUCCION_DELETE_SEGMENT,
    INSTRUCCION_EXIT,
    INSTRUCCION_error,
    INSTRUCCION_YIELD
} t_tipo_instruccion;

typedef enum{

    REGISTRO_ax,
    REGISTRO_bx,
    REGISTRO_cx,
    REGISTRO_dx,
    REGISTRO_eax,
    REGISTRO_ebx,
    REGISTRO_ecx,
    REGISTRO_edx,
    REGISTRO_rax,
    REGISTRO_rbx,
    REGISTRO_rcx,
    REGISTRO_rdx,
    REGISTRO_null

}t_registro;

typedef struct  {
    t_tipo_instruccion tipoInstruccion;
    uint32_t operando1;
    uint32_t operando2;
    char* dispositivo;
    t_registro registro1;
    t_registro registro2;
   // char* toString;
}t_instruccion;


typedef struct{

    uint32_t operando1;
    uint32_t operando2;
    char* dispositivo;
    t_registro registro1;
    t_registro registro2;
    
} t_info_instruccion;

t_list* lista_de_instrucciones_buffer(t_buffer* bufferConInstrucciones, t_log * logger);

#endif