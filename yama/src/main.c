#include "main.h"

yama_t *config;
socket_t sockSRV;
socket_t sockFS;
int num_sig_job;

/*
 * Conectar con FS, considerar que si el handshake falla puede que esté en un estado no estable
 */
void conectar_con_yamafs(yama_t* config) {
	if((sockFS = socket_connect(config->yamafs_ip, config->yamafs_puerto)) == -1) {
		exit(EXIT_FAILURE);
	}

	if(!protocol_handshake_send(sockFS, YAMA)) {
		exit(EXIT_FAILURE);
	}
}

void crear_servidor(yama_t* config) {
	socket_t cli_sock;
	header_t cabecera;
	sockSRV = socket_init(NULL, config->puerto);
	while(true) {
		cli_sock = socket_accept(sockSRV); //TODO: cambiar a select

		if(!protocol_handshake_receive(cli_sock, &cabecera)) {
			socket_close(cli_sock);
			continue;
		}
		if(cabecera.process != MASTER) {
			socket_close(cli_sock);
			continue;
		}



		socket_close(cli_sock);
	}
	socket_close(sockSRV);
}

int main(int argc, char **argv) {
	config = config_leer("metadata");

	log_init(config->log_file, config->log_name, true);

	conectar_con_yamafs(config);

	crear_servidor(config);

	return EXIT_SUCCESS;
}
