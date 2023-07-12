#ifndef CPU_MEMORIA_ADAPTER_H
#define CPU_MEMORIA_ADAPTER_H
#include <stdint.h>
#include <math.h>
#include "cpu.h"

void cpu_escribir_en_memoria(int toSocket, char* contenidoAEscribir, t_cpu_pcb* pcb);
char* cpu_leer_en_memoria( int toSocket, t_cpu_pcb* pcb);
void cpu_mmu(int toSocket, uint32_t direccionLogica, t_list* tablaDeSegmento, t_cpu_pcb* pcb , uint32_t cantidadByte);

#endif