#include "main.h"

yamamaster_t *config;
socket_t sockYAMA;

socket_t conectar_con_yama(yamamaster_t *config) {
	socket_t sock;
	if((sock = socket_connect(config->yama_ip, config->yama_puerto)) == -1) {
		exit(EXIT_FAILURE);
	}

	if(!protocol_handshake_send(sock, MASTER)) {
		exit(EXIT_FAILURE);
	}
	header_t header;
	if(!protocol_handshake_receive(sock, &header)) {
		exit(EXIT_FAILURE);
	}
	return sock;
}

#define BUFFER_CAPACITY 1024
int main(int argc, char **argv) {
	config = config_leer("metadata");

	log_init(config->log_file, config->log_name, true);

	sockYAMA = conectar_con_yama(config);

	unsigned char buffer[BUFFER_CAPACITY];
	int s = serial_pack(buffer, "h", 8);
	packet_t paquete;
	paquete.header.process = MASTER;
	paquete.header.operation = OP_MASTER_INICIAR_TAREA;
	paquete.header.size = s;
	paquete.payload = buffer;

	protocol_send(sockYAMA, &paquete);
	printf("envio de datos por protocolo con payload = %d\n", s);

	return EXIT_SUCCESS;
}
