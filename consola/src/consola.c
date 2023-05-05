#include "../include/consola.h"
#include "../include/consola_config.h"



t_log* consolaLogger;
t_config* consolaConfig;
extern t_consola_config *configDeKernel;




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

    char* pathArchivoInstrucciones = string_duplicate(argv[2]);
    consola_enviar_instrucciones_a_kernel(pathArchivoInstrucciones, kernelSocket, consolaLogger);
    free(pathArchivoInstrucciones);
    
    uint32_t idProceso = receive_pid_kernel(kernelSocket, consolaLogger);
    log_info(consolaLogger, "Se recibio el PID %d", idProceso);

    //wait_kernel_response(kernelSocket, idProceso, consolaConfig, consolaLogger);

    //consola_destroy(consolaConfig, consolaLogger);

 

   return 0;
}

void check_arguments(int argc, t_log* consolaLogger)
{
    if (argc != 3) { 
        
        log_error(consolaLogger, "Cantidad de argumentos inválida.\nArgumentos: <pathArchivoConfiguracion> <pathInstrucciones>");
        log_destroy(consolaLogger);
        
        exit(EXIT_FAILURE);
    }
}

void consola_enviar_instrucciones_a_kernel(const char *pathArchivoInstrucciones, const int kernelSocket, t_log *consolaLogger)
{
    t_buffer *instructionsBuffer = buffer_create();
    
    if(!consola_parser_parse_instructions(instructionsBuffer, pathArchivoInstrucciones, consolaLogger)) {

        stream_send_empty_buffer(kernelSocket, HEADER_error);
        log_error(consolaLogger, "Ocurrio un error en el parseo de las instrucciones. Finalizando consola...");
        log_destroy(consolaLogger);
        exit(EXIT_FAILURE);
    }

    stream_send_buffer(kernelSocket, HEADER_lista_instrucciones, instructionsBuffer);
    
    log_info(consolaLogger, "Se envía la lista de instrucciones al Kernel");
    
    buffer_destroy(instructionsBuffer);
    return;
} 

uint32_t receive_pid_kernel(const int kernelSocket,  t_log* consolaLogger)
{
    t_header kernelResponse;
    uint32_t idProcesoTemp = 0;

    kernelResponse = stream_recv_header(kernelSocket);
    
    if (kernelResponse != HEADER_pid) {
        
        log_error(consolaLogger, "Error al intentar recibir el PID");
       // consola_destroy(consolaConfig, consolaLogger);
        exit(EXIT_FAILURE);
    }

    t_buffer *bufferPID = buffer_create();
    stream_recv_buffer(kernelSocket, bufferPID);
    buffer_unpack(bufferPID, &idProcesoTemp, sizeof(idProcesoTemp));
    buffer_destroy(bufferPID);
    
    return idProcesoTemp;
}