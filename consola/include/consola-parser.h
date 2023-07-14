#ifndef CONSOLA_PARSER_H
#define CONSOLA_PARSER_H

//Standard libraries
#include <errno.h>
#include <stdlib.h>
#include <string.h>

//Commons libaries
#include <commons/log.h>
#include <commons/string.h>

//Static libraries
#include "../../utils/src/utils/instrucciones.h"

//Internal libraries consola module
#include "consola_serializar.h" 

//#include <consola-estructuras.h>
#include "../../utils/src/utils/conexiones.h"
#include "../../utils/src/utils/serializacion.h"
#include "../../utils/src/utils/commons-nuestras.h" //este

#define MAX_LENGTH_INSTRUCTION 50

bool consola_parser_parse_instructions(t_buffer *buffer, const char *pathInstrucciones, t_log *consolaLogger);
bool es_instruccion_valida(char* instruccion);
bool es_instruccion_con_un_argumentos(char* identificadorInstruccion);
bool es_instruccion_con_dos_argumentos(char* identificadorInstruccion);
bool es_instruccion_con_tres_argumentos(char* identificadorInstruccion);
t_tipo_instruccion obtener_tipo_instruccion(char* instruccion);
t_registro obtener_registro(char* registro);
int cantidad_argumentos(char** vectorInstruccion);

#endif