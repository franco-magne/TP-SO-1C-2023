#include <../include/fileSystem.h>

t_log* fs_logger;
t_config* fs_config;
t_config* superbloque_config;
t_config* fcb_config;
t_filesystem* fs;

int main() {
   
   fs_logger = log_create(FS_LOG_UBICACION, FS_PROCESS_NAME, true, LOG_LEVEL_INFO);
   fs_config = config_create(FS_CONFIG_UBICACION);
   superbloque_config = config_create(FS_SUPERBLOQUE_UBICACION);
   fcb_config = config_create(FS_FCB_UBICACION);

   // TODO: LLAMAR ACA A UNA FUNCION QUE INICIE LOS SEMAFOROS QUE VAYA A NECESITAR.

   cargar_t_filesystem(fs_config, superbloque_config, fcb_config, fs);


   ///////////////////////////////// CONECTARSE A MEMORIA //////////////////////////////

   int fsSocketMemoria = conectar_a_servidor(fs->ip_memoria, fs->puerto_memoria);
   if (fsSocketMemoria == -1) {

      log_error(fs_logger, "Error al intentar establecer conexión inicial con módulo Memoria");
      log_destroy(fs_logger);

      return -2;
   }
   fs->socket_memoria = fsSocketMemoria;
   log_info(fs_logger, "Conexion con MEMORIA establecida");

   levantar_bitmap(fs, fs_logger);
   crear_estructuras_administrativas(fs, fs_logger);


   ///////////////////////////////// CREA SERVIDOR PARA KERNEL /////////////////////////

   int serverFS = iniciar_servidor(fs->ip_memoria, fs->puerto_escucha); // DUDA: ¿INICIAR SERVIDOR NO DEBERIA RECIBIR SOLO EL PUERTO DE ESCUCHA?
   log_info(fs_logger, "Servidor FILESYSTEM listo para recibir a KERNEL\n");
   
   while (fs_escuchando_en(serverFS, fs)); // Escucho a Kernel


   config_destroy(fs_config);
   config_destroy(superbloque_config);
   config_destroy(fcb_config);

   return 0;
}
