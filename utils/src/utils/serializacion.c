#include "serializacion.h"

///////////////////////////////////////////////////////// BUFFER /////////////////////////////////////////

t_buffer* buffer_create(void) 
{
    t_buffer* self = malloc(sizeof(*self));
    self->size = 0;
    self->stream = NULL;

    return self;
}

t_buffer* buffer_create_copy(t_buffer* bufferToCopy) 
{
    t_buffer* self = malloc(sizeof(*self));
    self->size = bufferToCopy->size;
    self->stream = malloc(self->size);
    memcpy(self->stream, bufferToCopy->stream, self->size);

    return self;
}

void buffer_destroy(t_buffer* self) 
{
    free(self->stream);
    free(self);
}

void buffer_pack(t_buffer* self, void* streamToAdd, int size) 
{
    self->stream = realloc(self->stream, self->size + size);
    memcpy(self->stream + self->size, streamToAdd, size);
    self->size += size;
}

t_buffer* buffer_unpack(t_buffer* self, void* dest, int size) {

    if (self->stream == NULL || self->size == 0 || self->size < size) {
        puts("\e[0;31mbuffer_unpack: Error en el desempaquetado del buffer\e[0m");
        exit(-1);
    }

    memcpy(dest, self->stream, size);
    memmove(self->stream, self->stream + size, self->size - size);
    self->size -= size;
    self->stream = realloc(self->stream, self->size);

    return self;
}

void buffer_pack_string(t_buffer* self, char* stringToAdd) {

    if (stringToAdd == NULL) {
        char* cadenaVacia = "vacio";
        buffer_pack_string(self, cadenaVacia);
    } else {
        uint32_t length = strlen(stringToAdd) + 1;
        buffer_pack(self, &length, sizeof(length));
        self->stream = realloc(self->stream, self->size + length);
        memcpy(self->stream + self->size, stringToAdd, length);
        self->size += length;
    }
}

char* buffer_unpack_string(t_buffer* self) 
{
    char* str;
    uint32_t length;
    buffer_unpack(self, &length, sizeof(length));
    str = malloc(length);
    buffer_unpack(self, str, length);

    return str;
}

void buffer_peek(t_buffer* self, void* dest, int size, int offset) {
    if (self->stream == NULL || offset + size > self->size) {
        puts("\e[0;31mbuffer_peek: Error al acceder al buffer\e[0m");
        exit(-1);
    }
    memcpy(dest, self->stream + offset, size);
}

///////////////////////////////////////////////////////// STREAM /////////////////////////////////////////

static void stream_send(int toSocket, void* streamToSend, uint32_t bufferSize) {

    uint8_t header = 0;
    uint32_t size = 0;
    ssize_t bytesSent = send(toSocket, streamToSend, sizeof(header) + sizeof(size) + bufferSize, 0);
    
    if (bytesSent == -1) {
        printf("\e[0;31m__stream_send: Error en el envío del buffer [%s]\e[0m\n", strerror(errno));
    }
}

static void* stream_create(uint8_t header, t_buffer* buffer) {

    void* streamToSend = malloc(sizeof(header) + sizeof(buffer->size) + buffer->size);
    int offset = 0;

    memcpy(streamToSend + offset, &header, sizeof(header));
    offset += sizeof(header);
    memcpy(streamToSend + offset, &(buffer->size), sizeof(buffer->size));
    offset += sizeof(buffer->size);
    memcpy(streamToSend + offset, buffer->stream, buffer->size);
    
    return streamToSend;
}

void stream_send_buffer(int toSocket, uint8_t header, t_buffer* buffer) {

    void* stream = stream_create(header, buffer);
    stream_send(toSocket, stream, buffer->size);
    free(stream);
}

void stream_send_empty_buffer(int toSocket, uint8_t header) {
    
    t_buffer* emptyBuffer = buffer_create();
    stream_send_buffer(toSocket, header, emptyBuffer);
    buffer_destroy(emptyBuffer);
}

void stream_recv_buffer(int fromSocket, t_buffer* destBuffer) {

    ssize_t msgBytes = recv(fromSocket, &(destBuffer->size), sizeof(destBuffer->size), 0);
    
    if (msgBytes == -1) {
        printf("\e[0;31mstream_recv_buffer: Error en la recepción del buffer [%s]\e[0m\n", strerror(errno));
    } else if (destBuffer->size > 0) {
        destBuffer->stream = malloc(destBuffer->size);
        recv(fromSocket, destBuffer->stream, destBuffer->size, 0);
    }
}

void stream_recv_empty_buffer(int fromSocket) {

    t_buffer* buffer = buffer_create();
    stream_recv_buffer(fromSocket, buffer);
    buffer_destroy(buffer);
}

uint8_t stream_recv_header(int fromSocket) {

    uint8_t header;
    ssize_t msgBytes = recv(fromSocket, &header, sizeof(header), 0);

    if (msgBytes == -1) {
        printf("\e[0;31mstream_recv_buffer: Error en la recepción del header [%s]\e[0m\n", strerror(errno));
    }

    return header;
}
