#include "../include/consola_serializar.h"

void consola_serializer_pack_no_args(t_buffer* buffer, t_tipo_instruccion instruccion) 
{
    buffer_pack(buffer, &instruccion, sizeof(instruccion));
}

void consola_serializer_pack_one_integer_args(t_buffer* buffer, t_tipo_instruccion instruccion, uint32_t op1) 
{
    consola_serializer_pack_no_args(buffer, instruccion);
    buffer_pack(buffer, &op1, sizeof(op1));
}

void consola_serializer_pack_one_register_args(t_buffer* buffer, t_tipo_instruccion instruccion, t_registro registro) 
{
    consola_serializer_pack_no_args(buffer, instruccion);
    buffer_pack(buffer, &registro, sizeof(registro));
}

void consola_serializer_pack_one_string_args(t_buffer* buffer, t_tipo_instruccion instruccion, char* tipoDispositivoIO) 
{
    consola_serializer_pack_no_args(buffer, instruccion);
    buffer_pack_string(buffer, tipoDispositivoIO);
}

void consola_serializer_pack_three_args(t_buffer* buffer, t_tipo_instruccion instruccion, char* tipoDispositivoIO, uint32_t op1, uint32_t op2) 
{
    consola_serializer_pack_no_args(buffer, instruccion);
    buffer_pack_string(buffer, tipoDispositivoIO);
    buffer_pack(buffer, &op1, sizeof(op1));
    buffer_pack(buffer, &op2, sizeof(op2));
}


void consola_serializer_pack_two_args(t_buffer* buffer, t_tipo_instruccion instruccion, void* arg1, void* arg2) 
{

    switch (instruccion)
    {
        // FALTA F_TRUNCATE, F_SEEK, CREATE_SEGMENT
        case INSTRUCCION_SET:
        case INSTRUCCION_MOV_IN:

            consola_serializer_pack_one_register_args(buffer, instruccion, *((t_registro*) arg1));
            buffer_pack(buffer, arg2, sizeof(uint32_t));
            break;

        case INSTRUCCION_MOV_OUT:
            consola_serializer_pack_one_integer_args(buffer, instruccion, *((uint32_t*) arg1));
            buffer_pack(buffer, arg2, sizeof(t_registro));
            break;

        default:
            
            break;
    }
}

