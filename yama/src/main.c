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
	header_t cabecera;
	packet_t paquete;
	socket_t cli_sock, cli_i;
	fd_set read_fdset;

	sockSRV = socket_listen(config->puerto);

	while(true) {
		if(!socket_select(&read_fdset)) break;

		for(cli_i = 0; cli_i <= socket_fdmax(); cli_i++) {
			if(!socket_fdisset(cli_i, &read_fdset)) continue;

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
				if(!protocol_handshake_send(cli_sock, YAMA)) {
					socket_close(cli_sock);
					continue;
				}

				socket_fdset(cli_sock);
			}
			else {
				if(!protocol_receive(cli_i, &paquete)) {
					socket_close(cli_i);
					socket_fdclear(cli_i);
					continue;
				}
				switch(paquete.header.operation) {
					case OP_MASTER_INICIAR_TAREA: {
						int n;
						serial_unpack(paquete.payload, "h", &n);
						break;
					}
				}
			}
		}
	}
	socket_close(sockSRV);
}

int main(int argc, char **argv) {
	config = config_leer("metadata");

	log_init(config->log_file, config->log_name, true);

	//sockFS = conectar_con_yamafs(config);

	crear_servidor(config);

	return EXIT_SUCCESS;
}
