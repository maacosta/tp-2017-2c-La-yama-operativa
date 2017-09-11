#include "protocol.h"

#define HEADER_SIZE 6

static size_t send_message(message_t message, socket_t sockfd) {
	unsigned char buffer[HEADER_SIZE];
	size_t size = serial_pack(buffer, "CCL", message.header.process, message.operation, message.size);
	return socket_send_bytes(buffer, size, sockfd);
}

size_t protocol_handshake_send(socket_t sockfd, header_t header) {
	message_t message;
	message.header = header;
	message.operation = OP_HANDSHAKE;
	message.size = 0;
	return send_message(message, sockfd);
}

static size_t receive_message(message_t message, socket_t sockfd) {
	unsigned char buffer[HEADER_SIZE];
	size_t size = socket_receive_bytes(buffer, HEADER_SIZE, sockfd);
	if(size > 0) {
		serial_unpack(buffer, "CCL", &message.header.process, &message.operation, &message.size);
	}
	return size;
}

size_t protocol_handshake_receive(socket_t sockfd, header_t header) {
	message_t message = {0};
	size_t size = receive_message(message, sockfd);
	if(size > 0 && message.operation == OP_HANDSHAKE) {
		header = message.header;
		log_msg_info("Handshake con %s exitoso", message.header.process);
	} else {
		log_msg_error("Error en handshake con %s", message.header.process);
	}
	return size;
}

