#include "main.h"

unsigned char buffer[4096];
yamamaster_t *config;
socket_t sockYAMA;
char *p_transformador, *p_reductor, *p_origen, *p_destino;

void validar_parametros(int argc, char **argv) {
	if(argc != 5) {
		log_msg_error("Debe indicar 4 parametros: transformador reductor origen destino");
		exit(EXIT_FAILURE);
	}
	p_transformador = argv[1];
	p_reductor = argv[2];
	p_origen = argv[3];
	p_destino = argv[4];
	log_msg_info("Parametros %s %s %s %s", p_transformador, p_reductor, p_origen, p_destino);
}

socket_t conectar_con_yama(yamamaster_t *config) {
	socket_t sock;
	if((sock = socket_connect(config->yama_ip, config->yama_puerto)) == -1) {
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

socket_t conectar_con_worker(char *ip, char *puerto) {
	socket_t sock;
	if((sock = socket_connect(ip, puerto)) == -1) {
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

int main(int argc, char **argv) {
	global_set_process(MASTER);
	validar_parametros(argc, argv);
	config = config_leer("metadata");
	log_init(config->log_file, config->log_name, true);

	sockYAMA = conectar_con_yama(config);

	ejecutar_transformacion(sockYAMA, p_transformador, p_origen);

	ejecutar_reduccion(sockYAMA, p_reductor);

	ejecutar_reduccion_global(sockYAMA, p_reductor);

	ejecutar_almacenamiento(sockYAMA, p_destino);

	return EXIT_SUCCESS;
}
