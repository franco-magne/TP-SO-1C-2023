#include <../include/memoria-atender-cpu.h>

extern t_log *memoriaLogger;
 pthread_mutex_t mutexTest = PTHREAD_MUTEX_INITIALIZER; //extern

void atender_peticiones_cpu(int socketCpu) {
    uint8_t header;
    while ((header = stream_recv_header(socketCpu)) != -1) {
        t_buffer* buffer = buffer_create();
        stream_recv_buffer(socketCpu, buffer);

        switch (header) {
            case HEADER_marco: {
                uint32_t num_segmento;
                uint32_t pid;

                buffer_unpack(buffer, &num_segmento, sizeof(num_segmento));
                buffer_unpack(buffer, &pid, sizeof(pid));

                log_info(memoriaLogger, "PID <%i> : Num Segmento <%i> ", pid, num_segmento);

                t_buffer* bufferMarco = buffer_create();
                uint32_t marco = 6;
                buffer_pack(bufferMarco, &marco, sizeof(marco));
                stream_send_buffer(socketCpu, HEADER_marco, bufferMarco);
                buffer_destroy(bufferMarco);

                break;
            }

            case HEADER_movin: {
                uint32_t dirFisica;
                buffer_unpack(buffer, &dirFisica, sizeof(dirFisica));

                log_info(memoriaLogger, "Direccion Fisica + Desplazamiento : <%i> ", dirFisica);

                t_buffer* bufferContenido = buffer_create();
                uint32_t contenido = 20;
                buffer_pack(bufferContenido, &contenido, sizeof(contenido));

                stream_send_buffer(socketCpu, HEADER_movin, bufferContenido);
                buffer_destroy(bufferContenido);

                break;
            }

            case HEADER_movout:
                // Procesar solicitud de CPU - caso HEADER_movout
                break;

            default:
                // Tipo de solicitud desconocido
                break;
        }
    }
}