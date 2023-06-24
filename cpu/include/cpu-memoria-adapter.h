#ifndef CPU_MEMORIA_ADAPTER_H
#define CPU_MEMORIA_ADAPTER_H
#include <stdint.h>
#include <math.h>
#include "cpu.h"

void cpu_escribir_en_memoria(int toSocket, uint32_t direccionAEscribir, char* contenidoAEscribir, uint32_t pid);
char* cpu_leer_en_memoria( int toSocket, uint32_t direccionALeer, uint32_t pid );
uint32_t cpu_obtener_marco(int toSocket, uint32_t direccionLogica, uint32_t pid);

#endif