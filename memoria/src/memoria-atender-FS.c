#include <../include/memoria-atender-FS.h>

pthread_mutex_t mutexMemoriaData = PTHREAD_MUTEX_INITIALIZER; //extern

void atender_peticiones_fileSystem(int socketFS) {
    uint8_t header;
    for (;;) {
        header = stream_recv_header(socketFS);
        pthread_mutex_lock(&mutexMemoriaData);
        t_buffer* buffer = buffer_create();
        stream_recv_buffer(socketFS, buffer);

        switch (header) {
            case HEADER_f_read:{
                break;
            }
            case HEADER_f_write:{
                break;
            }
            default:
                break;
        }
    }
}