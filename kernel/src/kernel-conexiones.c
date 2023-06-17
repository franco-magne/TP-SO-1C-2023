
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
    log_info(kernelLogger,"%i",kernel_config_get_socket_dispatch_cpu(kernelConfig));
    stream_send_empty_buffer(socketCpuDispatch, HANDSHAKE_dispatch);
    uint8_t cpuDispatchResponse = stream_recv_header(socketCpuDispatch);
    stream_recv_empty_buffer(socketCpuDispatch);

    if (cpuDispatchResponse != HANDSHAKE_ok_continue) {
        
        log_error(kernelLogger, "Error al intentar establecer conexión con CPU Dispatch");
       // kernel_destroy(kernelConfig, kernelLogger);
        exit(EXIT_FAILURE);
    }
    log_info(kernelLogger, "Conexión con CPU por canal Dispatch establecida");

    return socketCpuDispatch;
}