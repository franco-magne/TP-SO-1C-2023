#include <stdio.h>
#include "conexiones.h"

int main() {
   // printf() displays the string inside quotation
  
   char *kernelIP = "127.0.0.1";
    char *kernelPort = "8000";
    const int kernelSocket = conectar_a_servidor(kernelIP, kernelPort);
    if (kernelSocket == -1) {
        //log_error(consolaLogger, "Error al intentar establecer conexión inicial con módulo Kernel");
        //consola_destroy(consolaConfig, consolaLogger);
        printf("No se pudo conectar al kernel");
        return -1;
    }


   return 0;
}
