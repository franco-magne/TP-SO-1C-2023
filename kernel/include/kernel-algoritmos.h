#ifndef KERNEL_ALGORITMOS_H
#define KERNEL_ALGORITMOS_H

///////////////// BIBLOTECAS STANDARS /////////////////

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

////////////////// BIBLOTECAS COMMONS ////////////////
#include <commons/log.h>
#include <commons/config.h>

////////////////// BIBLOTECAS NUESTRAS ///////////////
#include "kernel-estado.h"
#include "pcb.h"
#include "kernel.h"
#include "../../utils/src/utils/commons-nuestras.h"

t_pcb* elegir_pcb_segun_fifo(t_estado* estado);
t_pcb* elegir_pcb_segun_hhrn(t_estado* estado);

#endif