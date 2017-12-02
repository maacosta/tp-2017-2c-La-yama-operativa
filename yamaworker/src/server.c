#include "server.h"

socket_t sockSRV;

int procesar_operaciones(socket_t cliente, yamaworker_t *config) {
	packet_t packet = protocol_packet_receive(cliente);
	if(packet.header.operation == OP_ERROR) {
		socket_close(cliente);
		exit(EXIT_FAILURE);
	}
	bool resultado;
	switch(packet.header.operation) {
		case OP_WRK_Iniciar_Transformacion:
			resultado = op_transformar(&packet, cliente, config);
			break;
		case OP_WRK_Iniciar_Reduccion:
			resultado = op_reduccion(&packet, cliente, config);
			break;
		case OP_WRK_Iniciar_Reduccion_Global:
			break;
		case OP_WRK_Iniciar_Almacenamiento_Final:
			break;
		default:
			log_msg_error("Operacion [ %d ] no contemplada en el contexto de ejecucion", packet.header.operation);
			protocol_packet_free(&packet);
			resultado = false;
	}
	if(!resultado)
		socket_close(cliente);

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
