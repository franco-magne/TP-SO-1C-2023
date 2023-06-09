#include <../include/memoria-atender-cpu.h>

void atender_peticiones_cpu(int socketCpu) {
    
    uint8_t header = stream_recv_header(socketCpu);
    for(;;){
        switch (header){
        case HEADER_marco : printf("hola");
        break;
        default:
        break;
        }
    }
}