#include "main.h"

yama_t *config;

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

	socket_t sockFS = conectar_con_yamafs(config);
	t_list *nodos = filesystem_obtener_nodos(sockFS);

	server_crear_yama(config, sockFS, nodos);

	filesystem_liberar_nodos(nodos);
	server_liberar();
	config_liberar(config);

	return EXIT_SUCCESS;
}
