#include "protocol.h"

#define HEADER_SIZE 8

header_t protocol_get_header(unsigned int operation, unsigned long size) {
	header_t header;
	header.process = global_get_process();
	header.operation = operation;
	header.size = size;
	return header;
}

packet_t protocol_get_packet(header_t header, unsigned char* buffer) {
	packet_t packet;
	packet.header = header;
	packet.payload = buffer;
	return packet;
}

static size_t send_header(socket_t sockfd, header_t header) {
	unsigned char buffer[HEADER_SIZE];
	size_t size = serial_pack(&buffer, "HHL", header.process, header.operation, header.size);
	log_msg_info("protocol | Envio de Header [ %s ]", buffer);
	return socket_send_bytes((void *)&buffer, size, sockfd);
}

bool protocol_handshake_send(socket_t sockfd) {
	header_t header = protocol_get_header(OP_HANDSHAKE, 0);
	size_t size = send_header(sockfd, header);
	if(size > 0) {
		log_msg_info("protocol | Handshake enviado a socket [ %d ]", sockfd);
	}
	else {
		log_msg_error("protocol | No se pudo enviar el handshake al socket [ %d ]", sockfd);
	}
	return size > 0;
}

header_t protocol_header_receive(socket_t sockfd) {
	unsigned char buffer[HEADER_SIZE];
	header_t header;
	size_t size = socket_receive_bytes((void *)&buffer, HEADER_SIZE, sockfd);
	if(size > 0) {
		serial_unpack(&buffer, "HHL", &header.process, &header.operation, &header.size);
		log_msg_info("protocol | Recepcion de Header [ %s ]", buffer);
	}
	else {
		header.operation = OP_ERROR;
		log_msg_error("protocol | Error en recepcion de Header");
	}
	return header;
}

bool protocol_handshake_receive(socket_t sockfd, header_t *header) {
	*header = protocol_header_receive(sockfd);
	bool r = header->operation == OP_HANDSHAKE;
	if(r) {
		log_msg_info("protocol | Handshake recibido de socket [ %d ] exitoso", sockfd);
	} else {
		log_msg_error("protocol | Error en handshake recibido del socket [ %d ]", sockfd);
	}
	return r;
}


packet_t protocol_packet_receive(socket_t sockfd) {
	packet_t packet;
	packet.header = protocol_header_receive(sockfd);
	if(packet.header.size > 0) {
		packet.payload = malloc(packet.header.size * sizeof(unsigned char));
		socket_receive_bytes(packet.payload, packet.header.size, sockfd);
	}
	return packet;
}

bool protocol_packet_send(socket_t sockfd, packet_t *packet) {
	size_t size = send_header(sockfd, packet->header);
	if(size <= 0) {
		log_msg_info("protocol | Error enviando datos al socket [ %d ]", sockfd);
	}
	if(packet->header.size > 0) {
		size += socket_send_bytes(packet->payload, packet->header.size, sockfd);
	}
	return size == HEADER_SIZE + packet->header.size;
}

void protocol_packet_free(packet_t *packet) {
	if(packet->header.size > 0) {
		free(packet->payload);
	}
}
