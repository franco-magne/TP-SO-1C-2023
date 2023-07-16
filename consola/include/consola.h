#ifndef CONSOLA_H
#define CONSOLA_H

//////////////////// INCLUDE DE BASE ////////////////
#include "../../utils/src/utils/conexiones.h"
#include "../../utils/src/utils/serializacion.h"
#include <commons/log.h>
#include <commons/config.h>
#include "consola_config.h"
#include <stdio.h>
#include "consola_serializar.h"
#include "consola-parser.h"
#include <readline/readline.h>

#define CONSOLA_CONFIG_UBICACION "config/consola.config"
#define CONSOLA_LOG_UBICACION "logs/consola.log"
#define CONSOLA_PROCESS_NAME "Consola"

void check_arguments(int argc, t_log* consolaLogger);
void consola_enviar_instrucciones_a_kernel(const char *pathArchivoInstrucciones, const int kernelSocket,  t_log *consolaLogger);
uint32_t receive_pid_kernel(const int kernelSocket,  t_log* consolaLogger);

#endif