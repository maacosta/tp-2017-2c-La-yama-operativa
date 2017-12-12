#include "server.h"

socket_t sockSRV;

void guardar_archivo_tmp(const char *path, const char *nombre_archivo, unsigned char *stream, ssize_t len, bool es_txt) {
	if(!global_get_dir_exist(path))
		global_create_dir(path);

	char *path_file = string_from_format("%s/%s", path, nombre_archivo);
	if(es_txt) global_create_txtfile(path_file, stream, len);
	else global_create_binfile(path_file, stream, len);
	free(path_file);
}

int procesar_operaciones(socket_t cliente, yamaworker_t *config) {
	packet_t packet = protocol_packet_receive(cliente);
	if(packet.header.operation == OP_ERROR) {
		socket_close(cliente);
		exit(EXIT_FAILURE);
	}
	bool resultado;
	switch(packet.header.process) {
	case MASTER:
		switch(packet.header.operation) {
		case OP_WRK_Iniciar_Transformacion:
			resultado = op_transformar(&packet, cliente, config);
			break;
		case OP_WRK_Iniciar_Reduccion:
			resultado = op_reduccion(&packet, cliente, config);
			break;
		case OP_WRK_Iniciar_Reduccion_Global:
			resultado = op_reduccion_global(&packet, cliente, config);
			break;
		case OP_WRK_Iniciar_Almacenamiento_Final:
			resultado = op_almacenamiento_final(&packet, cliente, config);
			break;
		default:
			log_msg_error("Operacion [ %d ] no contemplada en el contexto de ejecucion", packet.header.operation);
			protocol_packet_free(&packet);
			resultado = false;
		}
		break;
	case WORKER:
		switch(packet.header.operation) {
		case OP_WRK_Obtener_Archivo_Local:
			resultado = op_obtener_archivo_local(&packet, cliente, config);
			break;
		default:
			log_msg_error("Operacion Local [ %d ] no contemplada en el contexto de ejecucion", packet.header.operation);
			protocol_packet_free(&packet);
			resultado = false;
		}
		break;
	}
	if(!resultado)
		socket_close(cliente);

	log_msg_info("Procesamiento de operacion [ %d ] [ %s ]", packet.header.process, resultado ? "EXITOSO" : "FALLIDO");

	exit(EXIT_SUCCESS);
}

socket_t conectar_con(char *ip, char *puerto) {
	socket_t sock;
	if((sock = socket_connect(ip, puerto)) == -1) {
		return -1;
	}

	if(!protocol_handshake_send(sock)) {
		socket_close(sock);
		return -1;
	}
	header_t header;
	if(!protocol_handshake_receive(sock, &header)) {
		socket_close(sock);
		return -1;
	}
	return sock;
}

socket_t aceptar_conexion(socket_t sockSRV) {
	socket_t cliente = socket_accept(sockSRV);
	if(cliente == -1) return -1;

	header_t cabecera;
	if(!protocol_handshake_receive(cliente, &cabecera)) {
		socket_close(cliente);
		return -1;
	}
	if(cabecera.process != MASTER && cabecera.process != WORKER) {
		socket_close(cliente);
		return -1;
	}
	if(!protocol_handshake_send(cliente)) {
		socket_close(cliente);
		return -1;
	}

	return cliente;
}

void server_crear(yamaworker_t *config) {
	socket_t cli_i;
	int pid;

	sockSRV = socket_init(NULL, config->puerto_nodo);

	while(true) {
		cli_i = aceptar_conexion(sockSRV);

		if(cli_i != -1) {
			if((pid = fork()) == 0) {
				//es el proceso hijo, procesar operaciones
				procesar_operaciones(cli_i, config);
			} //el else seria la logica del padre
		}
	}
	socket_close(sockSRV);
}
