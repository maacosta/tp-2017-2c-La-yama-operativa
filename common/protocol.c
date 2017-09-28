#include "protocol.h"

#define HEADER_SIZE 6

header_t protocol_get_header(unsigned char operation) {
	header_t header;
	header.process = global_get_process();
	header.operation = operation;
	header.size = 0;
	return header;
}
static size_t send_header(socket_t sockfd, header_t header) {
	unsigned char buffer[HEADER_SIZE];
	size_t size = serial_pack(buffer, "CCL", header.process, header.operation, header.size);
	return socket_send_bytes(buffer, size, sockfd);
}

bool protocol_handshake_send(socket_t sockfd) {
	header_t header = protocol_get_header(OP_HANDSHAKE);
	size_t size = send_header(sockfd, header);
	if(size > 0) {
		log_msg_info("Handshake enviado a socket %d", sockfd);
	}
	else {
		log_msg_error("No se pudo enviar el handshake al socket %d", sockfd);
	}
	return size > 0;
}

static size_t receive_header(socket_t sockfd, header_t *header) {
	unsigned char buffer[HEADER_SIZE];
	size_t size = socket_receive_bytes(buffer, HEADER_SIZE, sockfd);
	if(size > 0) {
		serial_unpack(buffer, "CCL", &header->process, &header->operation, &header->size);
	}
	return size;
}

bool protocol_handshake_receive(socket_t sockfd, header_t *header) {
	size_t size = receive_header(sockfd, header);
	bool r = size > 0 && header->operation == OP_HANDSHAKE;
	if(r) {
		log_msg_info("Handshake con socket %d exitoso", sockfd);
	} else {
		log_msg_error("Error en handshake con socket %d", sockfd);
	}
	return r;
}


size_t protocol_receive(socket_t sockfd, packet_t *packet) {
	size_t size = receive_header(sockfd, &packet->header);
	if(size <= 0) {
		log_msg_error("Error recibiendo datos cabecera del socket %d", sockfd);
	}
	if(packet->header.size > 0) {
		packet->payload = malloc(packet->header.size * sizeof(unsigned char));
		size += socket_receive_bytes(packet->payload, packet->header.size, sockfd);
	}
	return size;
}

size_t protocol_send(socket_t sockfd, packet_t *packet) {
	size_t size = send_header(sockfd, packet->header);
	if(size <= 0) {
		log_msg_info("Error enviando datos al socket %d", sockfd);
	}
	if(packet->header.size > 0) {
		size += socket_send_bytes(packet->payload, packet->header.size, sockfd);
	}
	return size;
}
