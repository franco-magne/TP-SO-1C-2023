#include <../include/cpu.h>

t_log* cpuLogger;
t_cpu_config* cpuConfig;

int main() {  

    cpuLogger = log_create(CPU_LOG_UBICACION,CPU_PROCESS_NAME,true,LOG_LEVEL_INFO);
    char* pathArchivoConfiguracion;
    struct sockaddr cliente = {0};
    socklen_t len = sizeof(cliente);
    
    t_config* configIncial = config_create(CPU_CONFIG_UBICACION);
    cpuConfig = cpu_config_initializer(configIncial);
    inicio_cpu();

  

    ///////////////////////////////// CONECTARSE A MEMORIA //////////////////////////////
    
     int cpuSocketMemoria = conectar_a_servidor("127.0.0.1", "8002");
      if (cpuSocketMemoria == -1) {
        log_error(cpuLogger, "Error al intentar establecer conexión inicial con módulo Memoria");

        log_destroy(cpuLogger);

        return -2;
      }
        stream_send_empty_buffer(cpuSocketMemoria,HANDSHAKE_cpu);
        
        cpu_config_set_socket_memoria(cpuConfig,cpuSocketMemoria);
  
  ///////////////////////////////// CREA SERVIDOR PARA KERNEL /////////////////////////

  int serverCpu = inicializar_servidor_cpu_dispatch(cpuConfig,cpuLogger);
  aceptar_conexion_dispatch(serverCpu, &cliente, &len);
  atender_peticiones_de_kernel();
  
   return 0;
}
