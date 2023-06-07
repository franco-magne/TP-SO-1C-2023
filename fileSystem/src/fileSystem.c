#include <../include/fileSystem.h>

t_log* fs_logger;
t_config* fs_config;
t_filesystem* fs;

int main() {
   
   fs_logger = log_create(FS_LOG_UBICACION, FS_PROCESS_NAME, true, LOG_LEVEL_INFO);
   fs_config = config_create(FS_CONFIG_UBICACION);

   cargar_t_filesystem(fs_config, fs);


   ///////////////////////////////// CONECTARSE A MEMORIA //////////////////////////////

   int fsSocketMemoria = conectar_a_servidor(fs->ip_memoria, fs->puerto_memoria);
   if (fsSocketMemoria == -1) {

      log_error(fs_logger, "Error al intentar establecer conexión inicial con módulo Memoria");
      log_destroy(fs_logger);

      return -2;
   }
   fs->socket_memoria = fsSocketMemoria;
   log_info(fs_logger, "Conexion con MEMORIA establecida");


   ///////////////////////////////// CREA SERVIDOR PARA KERNEL /////////////////////////

   int serverFS = iniciar_servidor(fs->ip_memoria, fs->puerto_escucha); // DUDA: ¿INICIAR SERVIDOR NO DEBERIA RECIBIR SOLO EL PUERTO DE ESCUCHA?
   log_info(fs_logger, "Servidor FILESYSTEM listo para recibir a KERNEL\n");
   int serverEspera = esperar_cliente(serverFS);



   return 0;
}
