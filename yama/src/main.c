#include "main.h"

yama_t *config;

int main(int argc, char **argv) {
	config = config_leer("metadata");

	log_init(config->log_file, config->log_name, true);

	socket_t srvfd = socket_listen(config->puerto);
	log_msg_info("escuchando");

	header_t cabecera;
	if(protocol_handshake_receive(srvfd, &cabecera)) {
		log_msg_info("handshake ok");
	}
	else {
		log_msg_error("ocurrió un error en handshake");
	}

	return 0;
}
