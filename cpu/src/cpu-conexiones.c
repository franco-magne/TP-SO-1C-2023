#include <../include/cpu-conexiones.h>

int inicializar_servidor_cpu_dispatch(t_cpu_config* cpuConfig, t_log* cpuLogger)
{
    int tempCpuSocketServerEscucha;

    tempCpuSocketServerEscucha = iniciar_servidor(cpu_config_get_ip_cpu(cpuConfig), cpu_config_get_puerto_dispatch(cpuConfig));

    if (tempCpuSocketServerEscucha == -1) {
        
        log_error(cpuLogger, "Error al intentar iniciar servidor dispatch del kernel");
        //cpu_destroy(cpuConfig, cpuLogger);
        exit(EXIT_FAILURE);
    }

    log_info(cpuLogger, "Se ha inicializado el servidor de escucha dispatch del kernel correctamente");

    return tempCpuSocketServerEscucha;
}

void aceptar_conexion_dispatch(const int socketEscuchaDispatch, struct sockaddr* cliente, socklen_t* len)
{
    // Conexión con Kernel en canal Dispatch
    const int kernelDispatchSocket = accept(socketEscuchaDispatch, cliente, len);
    
    if (kernelDispatchSocket == -1) {
        
        log_error(cpuLogger, "Error al intentar establecer conexión inicial módulo Kernel por canal Dispatch");
        //   cpu_destroy(cpuConfig, cpuLogger);
        exit(EXIT_FAILURE);
    }
    
    cpu_config_set_socket_dispatch(cpuConfig, kernelDispatchSocket);   

    t_handshake kernelDispatchResponse = stream_recv_header(kernelDispatchSocket);
    stream_recv_empty_buffer(kernelDispatchSocket);

    if (kernelDispatchResponse != HANDSHAKE_dispatch) {
        log_info(cpuLogger,"%i",kernelDispatchResponse );
        log_error(cpuLogger, "Error al intentar establecer Handshake inicial con módulo Kernel por canal Dispatch");
        //   cpu_destroy(cpuConfig, cpuLogger);
        exit(EXIT_FAILURE);
    }
    
    stream_send_empty_buffer(kernelDispatchSocket, HANDSHAKE_ok_continue);
    log_info(cpuLogger, "Conexión con Kernel por canal Dispatch establecida");
}
