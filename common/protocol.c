#include "protocol.h"

#define HEADER_SIZE 6

static size_t send_message(socket_t sockfd, header_t header) {
	unsigned char buffer[HEADER_SIZE];
	size_t size = serial_pack(buffer, "CCL", header.process, header.operation, header.size);
	return socket_send_bytes(buffer, size, sockfd);
}

size_t protocol_handshake_send(socket_t sockfd, process_t process) {
	header_t header;
	header.process = process;
	header.operation = OP_HANDSHAKE;
	header.size = 0;
	return send_message(sockfd, header);
}

static size_t receive_message(socket_t sockfd, header_t *header) {
	unsigned char buffer[HEADER_SIZE];
	size_t size = socket_receive_bytes(buffer, HEADER_SIZE, sockfd);
	if(size > 0) {
		serial_unpack(buffer, "CCL", &header->process, &header->operation, &header->size);
	}
	return size;
}

size_t protocol_handshake_receive(socket_t sockfd, header_t *header) {
	size_t size = receive_message(sockfd, header);
	if(size > 0 && header->operation == OP_HANDSHAKE) {
		log_msg_info("Handshake con %d exitoso", header->process);
	} else {
		log_msg_error("Error en handshake con %d", header->process);
	}
	return size;
}

