#include "server.h"

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

int procesar_operaciones(socket_t cliente) {
	packet_t packet = protocol_packet_receive(cliente);
	if(packet.header.operation == OP_ERROR) {
		socket_close(cliente);
		exit(EXIT_FAILURE);
	}
	int resultado;
	switch(packet.header.operation) {
		case OP_WRK_Iniciar_Transformacion:
			resultado = yw_operations(&packet, cliente);
			break;
		case OP_WRK_Iniciar_Reduccion:
			break;
		case OP_WRK_Iniciar_Reduccion_Global:
			break;
		case OP_WRK_Iniciar_Almacenamiento_Final:
			break;
		default:
			log_msg_error("Operacion [ %d ] no contemplada en el contexto de ejecucion", packet.header.operation);
			protocol_packet_free(&packet);
			exit(EXIT_FAILURE);
	}
	char buffer[SIZE];
	size_t size = serial_string_pack(&buffer, "h", resultado);
	header_t cabecera = protocol_get_header(packet.header.operation, size);
	packet_t paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(cliente, &paquete))
		exit(EXIT_FAILURE);

	protocol_packet_free(&packet);

	exit(EXIT_SUCCESS);
}

socket_t aceptar_conexion(socket_t sockSRV) {
	socket_t cliente = socket_accept(sockSRV);
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

socket_t create_server(yamaworker_t *config) {
	static socket_t sockMS,sockSRV;
	socket_t cli_i;
	int pid;

	sockSRV = socket_init(NULL, config->puerto); // hacer socket_init

	operation_init(config, sockSRV, sockMS);

	while(true) {
		cli_i = aceptar_conexion(sockSRV);

		if(cli_i != -1) {
			if((pid = fork()) == 0) {
				//es el proceso hijo, procesar operaciones
				procesar_operaciones(cli_i);
			} //el else seria la logica del padre
		}
	}
	socket_close(sockSRV);
	return sockMS;
}
