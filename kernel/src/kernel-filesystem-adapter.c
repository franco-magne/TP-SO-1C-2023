#include <../include/kernel-filesystem-adapter.h>

void instruccion_f_open(t_pcb* pcbAIniciar, t_kernel_config* kernelConfig, t_log* kernelLogger){

log_info(kernelLogger,"PID: <%i> - Abrir Archivo: <NOMBRE ARCHIVO>", pcb_get_pid(pcbAIniciar));

}

void instruccion_f_close(t_pcb* pcbAIniciar, t_kernel_config* kernelConfig, t_log* kernelLogger){

log_info(kernelLogger,"PID: <%i> - Cerrar Archivo: <NOMBRE ARCHIVO>", pcb_get_pid(pcbAIniciar));


}

void instruccion_f_truncate(t_pcb* pcbAIniciar, t_kernel_config* kernelConfig, t_log* kernelLogger){

log_info(kernelLogger,"PID: <%i> - Archivo: <NOMBRE ARCHIVO> - Tamaño: <TAMAÑO>", pcb_get_pid(pcbAIniciar));


}

void instruccion_f_seek(t_pcb* pcbAIniciar, t_kernel_config* kernelConfig, t_log* kernelLogger){
    
log_info(kernelLogger,"PID: <%i> - Actualizar puntero Archivo: <NOMBRE ARCHIVO> - Puntero <PUNTERO>", pcb_get_pid(pcbAIniciar));


}