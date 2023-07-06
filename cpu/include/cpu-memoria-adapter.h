#ifndef CPU_MEMORIA_ADAPTER_H
#define CPU_MEMORIA_ADAPTER_H
#include <stdint.h>
#include <math.h>
#include "cpu.h"

void cpu_escribir_en_memoria(int toSocket, uint32_t direccionAEscribir, char* contenidoAEscribir, t_cpu_pcb* pcb);
char* cpu_leer_en_memoria( int toSocket, uint32_t direccionALeer, t_cpu_pcb* pcb);
uint32_t cpu_mmu(int toSocket, uint32_t direccionLogica, t_list* tablaDeSegmento, uint32_t pid);

#endif