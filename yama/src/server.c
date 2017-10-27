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

static bool procesar_operaciones(socket_t cliente, yama_t *config, t_list *nodos) {
	packet_t packet = protocol_packet_receive(cliente);
	if(packet.header.operation == OP_ERROR) {
		socket_close(cliente);
		return false;
	}
	bool resultado;
	switch(packet.header.operation) {
	case OP_YAM_Enviar_Estado:
		resultado = jem_consultar(&packet, cliente, estados_master);
		break;
	case OP_YAM_Solicitar_Transformacion:
		resultado = transformacion_iniciar(&packet, cliente, sockFS, config, estados_master, nodos);
		break;
	case OP_YAM_Replanificar_Transformacion:
		;
		break;
	case OP_YAM_Solicitar_Reduccion:
		resultado = reduccion_iniciar(&packet, cliente, estados_master, nodos);
		break;
	case OP_YAM_Solicitar_Reduccion_Global:
		break;
	case OP_YAM_Solicitar_Almacenamiento_Final:
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

void server_crear_yama(yama_t* config, socket_t sockfs, t_list *nodos) {
	sockFS = sockfs;
	socket_t cli_i;
	fd_set read_fdset;

	sockSRV = socket_listen(config->puerto);

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
				if(!procesar_operaciones(cli_i, config, nodos)) {
					socket_fdclear(cli_i);
					continue;
				}
			}
		}
	}
	socket_close(sockSRV);
}

void generar_nombre_aleatorio(char *dest, size_t length) {
    char charset[] = "0123456789"
                     "abcdefghijklmnopqrstuvwxyz"
                     "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    while (length-- > 0) {
        size_t index = (double) rand() / RAND_MAX * (sizeof charset - 1);
        *dest++ = charset[index];
    }
    *dest = '\0';
}

void server_crear_nombre_archivo_temporal(char *dest) {
	char nom[NOMBRE_ARCHIVO_TMP];
	generar_nombre_aleatorio(&nom, 6);
	char *tmp = string_new();
	string_append(&tmp, "/tmp/");
	string_append(&tmp, &nom);
	strcpy(dest, tmp);
	free(tmp);
}

void server_liberar() {
	list_destroy_and_destroy_elements(estados_master, (void*)liberar_estado_master);
}
