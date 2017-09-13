#ifndef COMMON_PROTOCOL_H_
#define COMMON_PROTOCOL_H_

#include "log.h"
#include "global.h"
#include "serial.h"
#include "socket.h"

/* Operaciones del protocolo
 * Las respuestas viajan con el mismo codigo de operacion
 * el contexto y el payload si lo hubieran determina el flujo de ejecucion
 */
typedef enum {
	OP_ERROR = 0,		//Se utiliza para responder cualquier error, el detalle queda en el emisor
	OP_HANDSHAKE = 1
} operation_t;

typedef struct {
	unsigned char process;
	unsigned char operation;
	unsigned long size;
} header_t;

typedef struct {
	header_t header;
	unsigned char *payload;
} packet_t;

size_t protocol_handshake_send(socket_t sockfd, process_t process);

size_t protocol_handshake_receive(socket_t sockfd, header_t *header);

#endif /* COMMON_PROTOCOL_H_ */
