#ifndef COMMON_PROTOCOL_H_
#define COMMON_PROTOCOL_H_

#include "log.h"
#include "global.h"
#include "serial.h"
#include "socket.h"

typedef enum {
	OP_HANDSHAKE = 1
} operation_t;

typedef struct {
	process_t process;
} header_t;

typedef struct {
	header_t header;
	operation_t operation;
	unsigned long size;
} message_t;

typedef struct {
	message_t message;
	unsigned char *payload;
} packet_t;

#endif /* COMMON_PROTOCOL_H_ */
