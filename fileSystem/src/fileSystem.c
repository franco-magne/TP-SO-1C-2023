#include <../include/fileSystem.h>

t_filesystem* fs;
t_log* fs_logger;
t_config* fs_config;
t_config* superbloque_config;

int main() {
   
   /*------------------------------------------------------------------------- INICIO DE FILESYSTEM ----------------------------------------------------------------------------- */

   fs = malloc(sizeof(t_filesystem));   
   fs_config = config_create(FILESYSTEM_CONFIG_UBICACION);
   superbloque_config = config_create(SUPERBLOQUE_CONFIG_UBICACION);
   fs_logger = log_create(FS_LOG_UBICACION, FS_PROCESS_NAME, true, LOG_LEVEL_INFO);

   cargar_t_filesystem(fs_config, superbloque_config, fs);   
   fs->logger = fs_logger;
   imprimir_file_system();
   
	/*char* ipMemoria = readline(BOLD WHITE "ESCRIBIR LA IP DE MEMORIA -> " RESET);
   strcpy(fs->ip_memoria, ipMemoria);
   free(ipMemoria);
   
   char* ipFS = getIPAddress();
   printf(BOLD WHITE "IP DE FS: %s\n" RESET, ipFS);
   strcpy(fs->ip_escucha, ipFS);
   free(ipFS);
*/
/*
   RUTAS DEL CONFIG PARA LA ENTREGA FINAL

   PATH_SUPERBLOQUE=./fs/superbloque.dat
   PATH_BITMAP=./fs/bitmap.dat
   PATH_BLOQUES=./fs/bloques.dat
   PATH_FCB=./fs/fcb
*/   

   // ------------------------------------------------------------------------ CONECTARSE A MEMORIA ----------------------------------------------------------------------------
   
   int fsSocketMemoria = conectar_a_servidor(fs->ip_memoria, fs->puerto_memoria);
   if (fsSocketMemoria == -1) {

      log_error(fs->logger, "Error al intentar establecer conexión inicial con módulo Memoria");
      log_destroy(fs->logger);

      return -2;
   }     

   log_info(fs->logger, MAGENTA BOLD "Conexion con MEMORIA establecida");
   stream_send_empty_buffer(fsSocketMemoria, HANDSHAKE_fileSystem);
   fs->socket_memoria = fsSocketMemoria;   
  

   // ---------------------------------------------------------------- INICIO DE ESTRUCTURAS ADMINISTRATIVAS -------------------------------------------------------------------

   crear_directorios(fs);
   levantar_bitmap(fs);
   crear_superbloque_dat(fs, superbloque_config);
   levantar_archivo_de_bloques(fs);
   

   // ---------------------------------------------------------------------- CREA SERVIDOR PARA KERNEL --------------------------------------------------------------------------
   

   int serverFS = iniciar_servidor(fs->ip_escucha, fs->puerto_escucha);
   log_info(fs->logger, "Servidor FILESYSTEM listo para recibir a KERNEL...");
   
   while (fs_escuchando_en(serverFS, fs));


   // -------------------------------------------------------------------------- FIN DE FILESYSTEM ------------------------------------------------------------------------------


   log_info(fs->logger, "Finalizando FILESYSTEM...");

   cerrar_archivos();
   log_destroy(fs_logger);
   close(fs->socket_kernel);
   config_destroy(fs_config);
   config_destroy(superbloque_config); 

   return 0;
}
