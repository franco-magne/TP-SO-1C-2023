#ifndef SERIALIZACION_H
#define SERIALIZACION_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <commons/collections/list.h>

typedef struct {
    uint32_t size;
    void *stream;
} t_buffer;

/**
 * @brief Desempaqueta un string del buffer, leyendo y desempaquetando
 * previamente la longitud del string
 *
 * @example char* unString = buffer_unpack_string(buffer);
 */
char *buffer_unpack_string(t_buffer *buffer);

/**
 * @brief Crea un t_buffer*
 */
t_buffer *buffer_create(void);

/**
 * @brief Genera una nueva copia de un t_buffer*
 */
t_buffer *buffer_create_copy(t_buffer *bufferToCopy);

/**
 * @brief Destruye un t_buffer*
 */
void buffer_destroy(t_buffer *buffer);

/**
 * @brief Agrega al buffer un streamToAdd de tamaño size
 *
 * @example buffer_pack(buffer, unStream, sizeof(unStream));
 */
void buffer_pack(t_buffer *buffer, void *streamToAdd, int size);

/**
 * @brief Agrega al buffer un stringToAdd, empaquetando previamente
 * la longitud del string
 *
 * @example buffer_pack_string(buffer, stringAEmpaquetar);
 */
void buffer_pack_string(t_buffer *buffer, char *stringToAdd);

/**
 * @brief Desempaqueta size bytes del buffer desde el inicio,
 * almacenando lo desempaquetado en dest
 *
 * @example buffer_unpack(buffer, destino, sizeof(destino));
 */
t_buffer* buffer_unpack(t_buffer *buffer, void *dest, int size);


void buffer_peek(t_buffer* buffer, void* dest, int size, int offset);


///////////////////////////////////////////////////////// STREAM /////////////////////////////////////////
/**
 * @brief Recibe solamente el header del buffer enviado
 *
 * @example stream_recv_header(socketModuloRemitente);
 */
uint8_t stream_recv_header(int fromSocket);

/**
 * @brief Recibe solamente el payload (stream de bytes) del buffer enviado
 *
 * @example stream_recv_buffer(socketModuloRemitente, destBuffer);
 */
void stream_recv_buffer(int fromSocket, t_buffer* destBuffer);

/**
 * @brief En combinación con stream_recv_header/1, recibe un buffer que se
 * sabe que vendrá con un payload vacío. En otras palabras, útil para casos en
 * que se quiera recibir solamente un header (sin payload)
 *
 * @example stream_recv_header(socketModuloRemitente);
 *          stream_recv_empty_buffer(socketModuloRemitente);
 */
void stream_recv_empty_buffer(int fromSocket);

/**
 * @brief Envía un buffer previamente serializado, asignándole un header en su envío
 *
 * @example stream_send_buffer(socketModuloDestinatario, header, bufferSerializado);
 */
void stream_send_buffer(int toSocket, uint8_t header, t_buffer* buffer);

/**
 * @brief Envía solamente el header del buffer (sin payload)
 *
 * @example stream_send_empty_buffer(socketModuloDestinatario, header);
 */
void stream_send_empty_buffer(int toSocket, uint8_t header);


#endif