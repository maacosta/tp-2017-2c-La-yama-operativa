#include "main.h"

yama_t *config;

/*
 * Conectar con FS, considerar que si el handshake falla puede que esté en un estado no estable
 */
socket_t conectar_con_yamafs(yama_t* config) {
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

int main(int argc, char **argv) {
	global_set_process(YAMA);
	config = config_leer("metadata");
	log_init(config->log_file, config->log_name, true);

	//socket_t sockFS = conectar_con_yamafs(config);
	socket_t sockFS = 0;
	server_crear(config, sockFS);

	server_liberar();
	config_liberar(config);

	return EXIT_SUCCESS;
}
