#include <../include/consola.h>

t_log* consolaLogger;
t_config* consolaConfig;
extern t_consola_config *configDeKernel;


void check_arguments(int , t_log* );


int main(int argc, char *argv[]) { 
    
    consolaLogger = log_create(CONSOLA_LOG_UBICACION,CONSOLA_PROCESS_NAME,true,LOG_LEVEL_INFO);
    consolaConfig = config_create(CONSOLA_CONFIG_UBICACION);
    check_arguments(argc, consolaLogger); // chequea que se pasen todo los argumentos
    

    char *kernelIP = config_get_string_value(consolaConfig, "IP");
    char *kernelPort = config_get_string_value(consolaConfig,"PUERTO");
    
    const int kernelSocket = conectar_a_servidor(kernelIP, kernelPort);
    if (kernelSocket == -1) {
        log_error(consolaLogger, "Error al intentar establecer conexión inicial con módulo Kernel");
        //consola_destroy(consolaConfig, consolaLogger);
        
        log_destroy(consolaLogger);
        return -1;
    }

    t_buffer *buffer = buffer_create();
    //inicializar_config( argv[1] );
   
    //stream_send_empty_buffer(kernelSocket, HANDSHAKE_kernel);
    uint8_t kernelResponse = stream_recv_header(kernelSocket);
    //stream_recv_empty_buffer(kernelSocket);
    /*
    if (kernelResponse != HANDSHAKE_ok_continue) {
        log_error(consolaLogger, "Error al intentar establecer Handshake inicial con módulo Kernel");
        return -1;
    } 
    */
    int* testing = 2;

    buffer_pack(buffer, &testing, sizeof(int));
    stream_send_buffer(kernelSocket, HANDSHAKE_kernel, buffer);
    buffer_destroy(buffer);
    


 

   return 0;
}

void check_arguments(int argc, t_log* consolaLogger)
{
    if (argc != 1) { 
        
        log_error(consolaLogger, "Cantidad de argumentos inválida.\nArgumentos: <pathArchivoConfiguracion> <pathInstrucciones>");
        log_destroy(consolaLogger);
        
        exit(EXIT_FAILURE);
    }
}
