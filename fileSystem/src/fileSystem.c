#include <../include/fileSystem.h>

t_log* fs_logger;
t_config* fs_config;
t_config* superbloque_config;
t_filesystem* fs;

int main() {
   
   fs = malloc(sizeof(t_filesystem));
   fs_logger = log_create(FS_LOG_UBICACION, FS_PROCESS_NAME, true, LOG_LEVEL_INFO);
   fs_config = config_create(FS_CONFIG_UBICACION);
   superbloque_config = config_create(FS_SUPERBLOQUE_UBICACION);

   cargar_t_filesystem(fs_config, superbloque_config, fs);
   fs->logger = fs_logger;
   log_info(fs->logger, "FILESYSTEM iniciado");

   ///////////////////////////////// CONECTARSE A MEMORIA //////////////////////////////
   /*
   int fsSocketMemoria = conectar_a_servidor(fs->ip_memoria, fs->puerto_memoria);
   if (fsSocketMemoria == -1) {

      log_error(fs->logger, "Error al intentar establecer conexión inicial con módulo Memoria");
      log_destroy(fs->logger);

      return -2;
   }
   fs->socket_memoria = fsSocketMemoria;
   log_info(fs->logger, "Conexion con MEMORIA establecida");
   */
  
   crear_directorios(fs);
   levantar_bitmap(fs);
   crear_superbloque_dat(fs, superbloque_config);
   levantar_archivo_de_bloques(fs);


   ///////////////////////////////// CREA SERVIDOR PARA KERNEL /////////////////////////

   int serverFS = iniciar_servidor(fs->ip_memoria, fs->puerto_escucha);
   log_info(fs->logger, "Servidor FILESYSTEM listo para recibir a KERNEL");
   //log_info(fs->logger, "Socket servidor: %d", serverFS);
   
   while (fs_escuchando_en(serverFS, fs)); // Escucho a Kernel

   // ./bin/kernel.out kernel-config-inicial.config 

   config_destroy(fs_config);
   config_destroy(superbloque_config);
   cerrar_archivos();

   return 0;
}
