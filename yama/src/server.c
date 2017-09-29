#include "server.h"

static socket_t sockSRV;
static socket_t sockFS;

static socket_t aceptar_cliente(socket_t server) {
	socket_t cliente = socket_accept(server);
	if(cliente == -1) return -1;

	header_t cabecera;
	if(!protocol_handshake_receive(cliente, &cabecera)) {
		socket_close(cliente);
		return -1;
	}
	if(cabecera.process != MASTER) {
		socket_close(cliente);
		return -1;
	}
	if(!protocol_handshake_send(cliente)) {
		socket_close(cliente);
		return -1;
	}
	return cliente;
}

static bool procesar_operaciones(socket_t cliente) {
	packet_t packet = protocol_packet_receive(cliente);
	if(packet.header.operation == OP_ERROR) {
		socket_close(cliente);
		return false;
	}
	switch(packet.header.operation) {
		case OP_MASTER_TRANSFORMACION: {
			operation_iniciar_tarea(&packet);
			break;
		}
		default: {
			log_msg_error("Operacion [ %d ] no contemplada en el contexto de ejecucion", packet.header.operation);
			protocol_packet_free(&packet);
			return false;
		}
	}
	protocol_packet_free(&packet);
	return true;
}

void server_create(yama_t* config, socket_t sockfs) {
	sockFS = sockfs;
	socket_t cli_i;
	fd_set read_fdset;

	sockSRV = socket_listen(config->puerto);

	operation_init(config, sockSRV, sockFS);

	while(true) {
		if(!socket_select(&read_fdset)) break;

		for(cli_i = 0; cli_i <= socket_fdmax(); cli_i++) {
			if(!socket_fdisset(cli_i, &read_fdset)) continue;

			if(cli_i == sockSRV) {
				socket_t cli_sock = aceptar_cliente(sockSRV);
				if(cli_sock == -1) continue;
				socket_fdset(cli_sock);
			}
			else {
				if(!procesar_operaciones(cli_i)) {
					socket_fdclear(cli_i);
					continue;
				}
			}
		}
	}
	socket_close(sockSRV);
}
