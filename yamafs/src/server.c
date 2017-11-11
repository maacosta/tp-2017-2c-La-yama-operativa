#include "server.h"

pthread_t thSRV;
socket_t sockSRV;
static bool estado_estable;
static bool yama_conectada;
static bool esperar_DNs;

static socket_t aceptar_cliente(socket_t server) {
	socket_t cliente = socket_accept(server);
	if(cliente == -1) return -1;

	header_t cabecera;
	if(!protocol_handshake_receive(cliente, &cabecera)) {
		socket_close(cliente);
		return -1;
	}
	//no está en estado-estable -> solo permitir conectar datanodes
	if(!estado_estable && cabecera.process != DATANODE) {
		socket_close(cliente);
		return -1;
	}
	//está en estado-estable -> permitir conectar yama, workers y datanodes?
	if(estado_estable && cabecera.process != DATANODE && cabecera.process != YAMA && cabecera.process != WORKER) {
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
	if(packet.header.process == YAMA) {
		switch(packet.header.operation) {
		case OP_FSY_Informacion_Archivo:
			//resultado = jem_consultar(&packet, cliente, estados_master, nodos);
			break;
		case OP_FSY_Almacenar_Archivo:
			//resultado = transformacion_iniciar(&packet, cliente, sockFS, config, estados_master, nodos);
			break;
		case OP_FSY_Obtener_Nodos:
			;
			break;
		default:
			log_msg_error("Operacion [ %d ] no contemplada en el contexto de ejecucion", packet.header.operation);
			protocol_packet_free(&packet);
			return false;
		}
	}
	else if(packet.header.process == DATANODE) {
		switch(packet.header.operation) {
		case OP_FSY_Registrar_Nodo:
			//resultado = jem_consultar(&packet, cliente, estados_master, nodos);
			break;
		default:
			log_msg_error("Operacion [ %d ] no contemplada en el contexto de ejecucion", packet.header.operation);
			protocol_packet_free(&packet);
			return false;
		}
	}
	else if(packet.header.process == WORKER) {
		switch(packet.header.operation) {
		default:
			log_msg_error("Operacion [ %d ] no contemplada en el contexto de ejecucion", packet.header.operation);
			protocol_packet_free(&packet);
			return false;
		}
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

void server_crear_fs(yamafs_t *config, bool esperarDNs) {
	esperar_DNs = esperarDNs;
	estado_estable = false;
	yama_conectada = false;
	thSRV = thread_create(server_crear, config);
}

void server_liberar() {
	thread_join(thSRV);
}
