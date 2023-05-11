#include <../include/cpu.h>

t_log* cpuLogger;
t_config* cpuConfig;

int main() {  

    cpuLogger = log_create(CPU_LOG_UBICACION,CPU_PROCESS_NAME,true,LOG_LEVEL_INFO);
    cpuConfig = config_create(CPU_CONFIG_UBICACION);
    char *cpuIP = config_get_string_value(cpuConfig, "IP");
    char *cpuPort = config_get_string_value(cpuConfig,"PUERTO_CPU");

    ///////////////////////////////// CONECTARSE A MEMORIA //////////////////////////////
     int cpuSocketMemoria = conectar_a_servidor(cpuIP, cpuPort);
      if (cpuSocketMemoria == -1) {
        log_error(cpuLogger, "Error al intentar establecer conexión inicial con módulo Memoria");

        log_destroy(cpuLogger);

        return -2;
      }


   ///////////////////////////////// CREA SERVIDOR PARA KERNEL /////////////////////////

   int serverCpu = iniciar_servidor(cpuIP,"8001");
   log_info(cpuLogger,"Servidor kernel listo para recibir al modulo\n");
   int serverEspera = esperar_cliente(serverCpu);

  atender_peticiones_de_kernel();
  

   return 0;
}
