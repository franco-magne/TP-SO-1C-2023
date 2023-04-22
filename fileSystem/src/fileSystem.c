#include <../include/fileSystem.h>

t_log* fileSystemLogger;
t_config* fileSystemConfig;

int main() {
   
   fileSystemLogger = log_create(FS_LOG_UBICACION,FS_PROCESS_NAME,true,LOG_LEVEL_INFO);
   fileSystemConfig = config_create(FS_CONFIG_UBICACION);
   char *fileSystemIP = config_get_string_value(fileSystemConfig, "IP");
   char *fileSystemPort = config_get_string_value(fileSystemConfig,"PUERTO_MEMORIA");

   ///////////////////////////////// CONECTARSE A MEMORIA //////////////////////////////
   int fsSocketMemoria = conectar_a_servidor(fileSystemIP, fileSystemPort);
   if (fsSocketMemoria == -1) {
      log_error(fileSystemLogger, "Error al intentar establecer conexión inicial con módulo Memoria");

      log_destroy(fileSystemLogger);

      return -2;
   }


   ///////////////////////////////// CREA SERVIDOR PARA KERNEL /////////////////////////

   int serverFS = iniciar_servidor(fileSystemIP,"8003");
   log_info(fileSystemLogger,"Servidor kernel listo para recibir al modulo\n");
   int serverEspera = esperar_cliente(serverFS);



   return 0;
}
