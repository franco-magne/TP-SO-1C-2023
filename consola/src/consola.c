#include <../include/consola.h>

t_log* consolaLogger;
t_config* consolaConfig;



int main() { 
    
    consolaLogger = log_create(CONSOLA_LOG_UBICACION,CONSOLA_PROCESS_NAME,true,LOG_LEVEL_INFO);
    consolaConfig = config_create(MEMORIA_CONFIG_UBICACION);


    char *kernelIP = config_get_string_value(consolaConfig, "IP");
    char *kernelPort = config_get_string_value(consolaConfig,"PUERTO");
    
    const int kernelSocket = conectar_a_servidor(kernelIP, kernelPort);
    if (kernelSocket == -1) {
        log_error(consolaLogger, "Error al intentar establecer conexión inicial con módulo Kernel");
        //consola_destroy(consolaConfig, consolaLogger);
        
        log_destroy(consolaLogger);
        return -1;
    }


   return 0;
}
