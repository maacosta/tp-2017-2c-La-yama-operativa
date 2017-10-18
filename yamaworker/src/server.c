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

static bool procesar_operaciones(socket_t cliente) {
	packet_t packet = protocol_packet_receive(cliente);
	if(packet.header.operation == OP_ERROR) {
		socket_close(cliente);
		return false;
	}

	switch(packet.header.operation) {
		case OP_WRK_Iniciar_Transformacion:
			//yw_iniciar_transformacion(&packet, cliente);
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
			return false;
	}
	protocol_packet_free(&packet);
	return true;
}

socket_t escuchar_master(yamaworker_t *config, socket_t sockms) {
	static socket_t sockMS,sockSRV;
	sockMS = sockms;
	socket_t cli_i;
	fd_set read_fdset;

	sockSRV = socket_listen(config->puerto);

	operation_init(config, sockSRV, sockMS);

	while(true) {
		if(!socket_select(&read_fdset)) break;

		for(cli_i = 0; cli_i <= socket_fdmax(); cli_i++) {
			if(!socket_fdisset(cli_i, &read_fdset)) continue;

			if(cli_i == sockSRV) {
				socket_t cli_sock = aceptar_cliente(sockSRV);
				if(cli_sock == -1) continue;
				socket_fdset(cli_sock);
				//recibir mensaje
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
	return sockMS;
}






/*char* recibir_mensajes_en_socket(int socket) {
	char* buf = malloc(1000);
	while (1) {
		int bytesRecibidos = recv(socket, buf, 1000, 0);
		if (bytesRecibidos < 0) {
			perror("Ha ocurrido un error al recibir un mensaje");
			exit(EXIT_FAILURE);
		} else if (bytesRecibidos == 0) {
			printf("Se terminó la conexión en el socket %d\n", socket);
			close(socket);
			exit(EXIT_FAILURE);
		} else {
			//Recibo mensaje e informo
			buf[bytesRecibidos] = '\0';
			printf("Recibí el mensaje de %i bytes: ", bytesRecibidos);
			//puts(buf);
		}
	}
	//free(buf);
	return buf;
}*/


