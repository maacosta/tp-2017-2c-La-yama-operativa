#include "protocol.h"

#define HEADER_SIZE 8

void protocol_nombre_etapa(etapa_t etapa, char *nombre_etapa) {
	if(etapa == ETAPA_Transformacion) strcpy(nombre_etapa, "ETAPA_Transformacion");
	else if(etapa == ETAPA_Reduccion_Local) strcpy(nombre_etapa, "ETAPA_Reduccion_Local");
	else if(etapa == ETAPA_Reduccion_Global) strcpy(nombre_etapa, "ETAPA_Reduccion_Global");
	else if(etapa == ETAPA_Almacenamiento_Final) strcpy(nombre_etapa, "ETAPA_Almacenamiento_Final");
}

void protocol_nombre_estado(estado_t estado, char *nombre_etapa) {
	if(estado == ESTADO_En_Proceso) strcpy(nombre_etapa, "ESTADO_En_Proceso");
	else if(estado == ESTADO_Error_Replanifica) strcpy(nombre_etapa, "ESTADO_Error_Replanifica");
	else if(estado == ESTADO_Finalizado_OK) strcpy(nombre_etapa, "ESTADO_Finalizado_OK");
	else if(estado == ESTADO_Error) strcpy(nombre_etapa, "ESTADO_Error");
}

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
	size_t size = serial_pack((unsigned char*)&buffer, "HHL", header.process, header.operation, header.size);
	log_msg_info("protocol | Envio de Header [ p%d o%d s%d ]", header.process, header.operation, header.size);
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
		serial_unpack((char*)&buffer, "HHL", &header.process, &header.operation, &header.size);
		log_msg_info("protocol | Recepcion de Header [ p%d o%d s%d ]", header.process, header.operation, header.size);
	}
	else {
		header.operation = OP_ERROR;
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
