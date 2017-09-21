#include "main.h"

yama_t *config;
socket_t sockSRV;
socket_t sockFS;
int num_sig_job;

/*
 * Conectar con FS, considerar que si el handshake falla puede que esté en un estado no estable
 */
socket_t conectar_con_yamafs(yama_t* config) {
	socket_t sock;
	if((sock = socket_connect(config->yamafs_ip, config->yamafs_puerto)) == -1) {
		exit(EXIT_FAILURE);
	}

	if(!protocol_handshake_send(sock, YAMA)) {
		exit(EXIT_FAILURE);
	}
	header_t header;
	if(!protocol_handshake_receive(sock, &header)) {
		exit(EXIT_FAILURE);
	}
	return sock;
}

void crear_servidor(yama_t* config) {
	socket_t cli_sock, cli_i;
	header_t cabecera;
	fd_set active_fd_set, read_fd_set;

	sockSRV = socket_init(NULL, config->puerto);

	FD_ZERO(&active_fd_set);
	FD_SET(sockSRV, &active_fd_set);

	while(true) {
		read_fd_set = active_fd_set;
		if(select(sockSRV + 1, &read_fd_set, NULL, NULL, NULL) == -1) {
			break;
		}

		for(cli_i = 0; cli_i <= sockSRV + 1; cli_i++) {
			if(!FD_ISSET(cli_i, &read_fd_set)) continue;

			if(cli_i == sockSRV) {
				/* Connection request on original socket. */
				cli_sock = socket_accept(sockSRV);
				if(cli_sock == -1) continue;

				if(!protocol_handshake_receive(cli_sock, &cabecera)) {
					socket_close(cli_sock);
					continue;
				}
				if(cabecera.process != MASTER) {
					socket_close(cli_sock);
					continue;
				}

				FD_SET (cli_sock, &active_fd_set);
			}
			else {
				/* Data arriving on an already-connected socket. */
				/* leer sobre el socket i, cerrar si es necesario y sacarlo del fd
				if (read_from_client (cli_i) < 0) {
					close (cli_i);
					FD_CLR (cli_i, &active_fd_set);
				}
				*/
			}
		}
	}
	socket_close(sockSRV);
}

int main(int argc, char **argv) {
	config = config_leer("metadata");

	log_init(config->log_file, config->log_name, true);

	sockFS = conectar_con_yamafs(config);

	crear_servidor(config);

	return EXIT_SUCCESS;
}
