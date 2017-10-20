#include "server.h"

static socket_t sockSRV;
static socket_t sockFS;
t_list *estados_master;

static void liberar_estado_master(estado_master_t *estadoMaster) {
	free(estadoMaster);
}

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
		case OP_YAM_Solicitar_Transformacion:
			operation_solicitar_transformacion(&packet, estados_master, cliente);
			break;
		case OP_YAM_Enviar_Estado_Transformacion:
			break;
		case OP_YAM_Solicitar_Reduccion:
			break;
		case OP_YAM_Enviar_Estado_Reduccion:
			break;
		case OP_YAM_Solicitar_Reduccion_Global:
			break;
		case OP_YAM_Enviar_Estado_Reduccion_Global:
			break;
		case OP_YAM_Solicitar_Almacenamiento_Final:
			break;
		case OP_YAM_Enviar_Estado_Almacenamiento_Final:
			break;
		default:
			log_msg_error("Operacion [ %d ] no contemplada en el contexto de ejecucion", packet.header.operation);
			protocol_packet_free(&packet);
			return false;
	}
	protocol_packet_free(&packet);
	return true;
}

void server_crear(yama_t* config, socket_t sockfs, t_list *nodos) {
	sockFS = sockfs;
	socket_t cli_i;
	fd_set read_fdset;

	sockSRV = socket_listen(config->puerto);

	operation_init(config, sockSRV, sockFS);

	estados_master = list_create();
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

void server_liberar() {
	list_destroy_and_destroy_elements(estados_master, (void*)liberar_estado_master);
}
