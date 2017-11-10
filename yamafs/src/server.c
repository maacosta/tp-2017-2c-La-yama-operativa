#include "server.h"

pthread_t thSRV;
socket_t sockSRV;
bool fsOperativo;
bool yamaEstaConectada;

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

static bool procesar_operaciones(socket_t cliente, yamafs_t *config) {
	packet_t packet = protocol_packet_receive(cliente);
	if(packet.header.operation == OP_ERROR) {
		socket_close(cliente);
		return false;
	}
	bool resultado;
	switch(packet.header.operation) {
	case OP_YAM_Enviar_Estado:
		//resultado = jem_consultar(&packet, cliente, estados_master, nodos);
		break;
	case OP_YAM_Solicitar_Transformacion:
		//resultado = transformacion_iniciar(&packet, cliente, sockFS, config, estados_master, nodos);
		break;
	case OP_YAM_Replanificar_Transformacion:
		;
		break;
	case OP_YAM_Solicitar_Reduccion:
		//resultado = reduccion_iniciar(&packet, cliente, estados_master, nodos);
		break;
	case OP_YAM_Solicitar_Reduccion_Global:
		//resultado = reduccion_global_iniciar(&packet, cliente, estados_master, nodos);
		break;
	case OP_YAM_Solicitar_Almacenamiento_Final:
		//resultado = almacenamiento_iniciar(&packet, cliente, estados_master, nodos);
		break;
	default:
		log_msg_error("Operacion [ %d ] no contemplada en el contexto de ejecucion", packet.header.operation);
		protocol_packet_free(&packet);
		return false;
	}
	if(!resultado)
		socket_close(cliente);
	return resultado;
}

void server_crear(yamafs_t *config) {
	socket_t cli_i;
	fd_set read_fdset;

	sockSRV = socket_listen(config->puerto);

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
				if(!procesar_operaciones(cli_i, config)) {
					socket_fdclear(cli_i);
					continue;
				}
			}
		}
	}
	socket_close(sockSRV);
}

void server_crear_fs(yamafs_t *config) {
	thSRV = thread_create(server_crear, config);
}

void server_liberar() {
	thread_join(thSRV);
}
