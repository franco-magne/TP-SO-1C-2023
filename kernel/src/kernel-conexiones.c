
#include <../include/kernel-conexiones.h>

int conectar_a_servidor_cpu_dispatch( t_kernel_config* kernelConfig, t_log* kernelLogger)
{
    // Conexión con CPU en canal Dispatch
   
    const int socketCpuDispatch = conectar_a_servidor(kernel_config_get_ip_cpu(kernelConfig), kernel_config_get_puerto_cpu(kernelConfig));
    
    if (socketCpuDispatch == -1) {
        
        log_error(kernelLogger, "Error al intentar establecer conexión inicial módulo CPU por canal Dispatch");
        //kernel_destroy(kernelConfig, kernelLogger);
        exit(EXIT_FAILURE);
    }

    kernel_config_set_socket_dispatch_cpu(kernelConfig, socketCpuDispatch);
    stream_send_empty_buffer(socketCpuDispatch, HANDSHAKE_dispatch);
    uint8_t cpuDispatchResponse = stream_recv_header(socketCpuDispatch);
    stream_recv_empty_buffer(socketCpuDispatch);

    if (cpuDispatchResponse != HANDSHAKE_ok_continue) {
        
        log_error(kernelLogger, "Error al intentar establecer conexión con CPU Dispatch");
       // kernel_destroy(kernelConfig, kernelLogger);
        exit(EXIT_FAILURE);
    }
    log_info(kernelLogger, "\033[92m Conexion con CPU establecida - Socket <%i> \033[0m", socketCpuDispatch);

    return socketCpuDispatch;
}

int conectar_con_servidor_memoria(t_kernel_config* kernelConfig, t_log* kernelLogger){

    int kernelSocketMemoria = conectar_a_servidor(kernel_config_get_ip_memoria(kernelConfig), kernel_config_get_puerto_memoria(kernelConfig));
      if (kernelSocketMemoria == -1) {
        log_error(kernelLogger, "Error al intentar establecer conexión inicial con módulo Memoria");
        log_destroy(kernelLogger);

        return -2;
      } 

    stream_send_empty_buffer(kernelSocketMemoria, HANDSHAKE_kernel);
    kernel_config_set_socket_memoria(kernelConfig,kernelSocketMemoria);
    log_info(kernelLogger, "\033[92m Conexion con memoria establecida - Socket <%i> \033[0m", kernelSocketMemoria);

    return kernelSocketMemoria;
}

int conectar_con_servidor_file_system(t_kernel_config* kernelConfig, t_log* kernelLogger){
    
    int kernelSocketFS = conectar_a_servidor(kernel_config_get_ip_file_system(kernelConfig), kernel_config_get_puerto_file_system(kernelConfig));
    if (kernelSocketFS == -1) {
        log_error(kernelLogger, "Error al intentar establecer conexión inicial con módulo FILE_SYSTEM");

        log_destroy(kernelLogger);

    return -2;
    }

kernel_config_set_socket_file_system(kernelConfig,kernelSocketFS);
log_info(kernelLogger, "\033[92m Conexion con file-system establecida - Socket <%i> \033[0m", kernelSocketFS);

return kernelSocketFS;
}