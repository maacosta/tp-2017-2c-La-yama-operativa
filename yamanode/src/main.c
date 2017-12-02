#include "main.h"

yamaDataNodo_t *config;
socket_t sockFS;

socket_t conectar_con_yamafs(yamaDataNodo_t *config) {
	socket_t sock;
	if((sock = socket_connect(config->yamafs_ip, config->yamafs_puerto)) == -1) {
		exit(EXIT_FAILURE);
	}

	if(!protocol_handshake_send(sock)) {
		exit(EXIT_FAILURE);
	}
	header_t header;
	if(!protocol_handshake_receive(sock, &header)) {
		exit(EXIT_FAILURE);
	}
	return sock;
}

void atender_solicitudes(yamaDataNodo_t *config) {
	packet_t packet;
	header_t cabecera;
	packet_t paquete;
	size_t size;
	int num_bloque, len;
	unsigned char *bloque;

	//registrar
	char buffer[NOMBRE_NODO_SIZE + BLOQUE_SIZE_E + IP_SIZE + PUERTO_SIZE + 3];
	size = serial_string_pack(&buffer, "s h s s", config->nombre_nodo, memoria_obtener_tamanio() / BLOQUE_LEN, config->ip_nodo, config->puerto_nodo);
	cabecera = protocol_get_header(OP_FSY_Registrar_Nodo, size);
	paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(sockFS, &paquete))
		exit(EXIT_FAILURE);

	//atender solicitudes de yamafs
	while(true) {
		packet = protocol_packet_receive(sockFS);
		if(packet.header.operation == OP_ERROR) {
			socket_close(sockFS);
			exit(EXIT_FAILURE);
		}
		switch(packet.header.operation) {
		case OP_DND_Obtener_Bloque:
			serial_string_unpack(packet.payload, "h h", &num_bloque, &len);
			protocol_packet_free(&packet);

			bloque = memoria_obtener_bloque(num_bloque, len);

			cabecera = protocol_get_header(OP_DND_Obtener_Bloque, len);
			paquete = protocol_get_packet(cabecera, bloque);
			if(!protocol_packet_send(sockFS, &paquete))
				exit(EXIT_FAILURE);
			free(bloque);
			break;
		case OP_DND_Almacenar_Bloque:
			serial_string_unpack(packet.payload, "h", &num_bloque);
			protocol_packet_free(&packet);

			packet = protocol_packet_receive(sockFS);

			memoria_almacenar_bloque(num_bloque, packet.header.size, packet.payload);
			protocol_packet_free(&packet);
			break;
		default:
			log_msg_error("Operacion [ %d ] no contemplada en el contexto de ejecucion", packet.header.operation);
			protocol_packet_free(&packet);
			break;
		}
	}
}

int main(int argc, char **argv) {
	global_set_process(DATANODE);
	config = config_leer("metadata");
	log_init(config->log_file, config->log_name, true);

	memoria_abrir(config, false);

	sockFS = conectar_con_yamafs(config);

	if(sockFS != -1)
		atender_solicitudes(config);

	memoria_destruir();

	config_liberar(config);

	return EXIT_SUCCESS;
}
