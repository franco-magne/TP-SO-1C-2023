#ifndef CONSOLA_SERIALIZAR_H
#define CONSOLA_SERIALIZAR_H

//Standard libraries
#include <errno.h>
#include <stdlib.h>
#include <string.h>

//Commons libaries
#include <commons/log.h>
#include <commons/string.h>

//Static libraries

#include <utils/instrucciones.h>
#include "utils/commons-nuestras.h"
#include "../../utils/src/utils/serializacion.h"
#include "../../utils/src/utils/instrucciones.h"


void consola_serializer_pack_no_args(t_buffer* buffer, t_tipo_instruccion instruccion);
void consola_serializer_pack_one_integer_args(t_buffer* buffer, t_tipo_instruccion instruccion, uint32_t op1);
void consola_serializer_pack_one_register_args(t_buffer* buffer, t_tipo_instruccion instruccion, t_registro registro);
void consola_serializer_pack_one_string_args(t_buffer* buffer, t_tipo_instruccion instruccion, char* tipoDispositivoIO);
void consola_serializer_pack_two_args(t_buffer* buffer, t_tipo_instruccion instruccion, void* arg1, void* arg2);
void consola_serializer_pack_three_args(t_buffer* buffer, t_tipo_instruccion instruccion, char* tipoDispositivoIO, uint32_t op1, uint32_t op2);


#endif
