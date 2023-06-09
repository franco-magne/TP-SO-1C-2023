#include <../include/kernel-instrucciones.h>



void instruccion_exit(t_pcb* pcbAExit, t_estado* estadoExit){

                pcb_set_estado_actual(pcbAExit, EXIT);
                estado_encolar_pcb_atomic(estadoExit, pcbAExit);
                //stream_send_empty_buffer(pcb_get_socket(pcb), HEADER_proceso_terminado);
                sem_post(estado_get_sem(estadoExit));
                //pcb_set_proceso_bloqueado_o_terminado_atomic(pcb, true);
                //terminar_proceso(pcb);
            

}


