#include "main.h"

yama_t *config;

/*
 * Conectar con FS, considerar que si el handshake falla puede que esté en un estado no estable
 */
socket_t conectar_con_yamafs(yama_t* config) {
	socket_t sock;
	if((sock = socket_connect(config->yamafs_ip, config->yamafs_puerto)) == -1) {
		exit(EXIT_FAILURE);
	}

	if(!protocol_handshake_send(sock)) {
		exit(EXIT_FAILURE);
	}
	header_t header;
	if(!protocol_handshake_receive(sock, &header)) {
		exit(EXIT_FAILURE);
	}
	return sock;
}

t_list *obtener_nodos(socket_t sock) {
	header_t cabecera;
	packet_t paquete;
	size_t size;

	//enviar consulta de Nodos
	cabecera = protocol_get_header(OP_FSY_Obtener_Nodos, 0);
	paquete = protocol_get_packet(cabecera, NULL);
	if(!protocol_packet_send(sock, &paquete))
		exit(EXIT_FAILURE);

	//recibir Info de Nodos
	paquete = protocol_packet_receive(sock);
	if(paquete.header.operation == OP_ERROR)
		exit(EXIT_FAILURE);
	int cant_nodos;
	serial_unpack(paquete.payload, "h", &cant_nodos);
	protocol_packet_free(&paquete);

	//detalle de cada bloque
	t_list *nodos = list_create();
	int i;
	for(i = 0; i < cant_nodos; i++) {
		paquete = protocol_packet_receive(sock);
		if(paquete.header.operation == OP_ERROR)
			exit(EXIT_FAILURE);
		detalle_nodo_t *det = malloc(sizeof(detalle_nodo_t));
		serial_unpack(paquete.payload, "s s s", det->nodo, det->ip, det->puerto);
		protocol_packet_free(&paquete);
		list_add(nodos, det);
	}
	return nodos;
}

int main(int argc, char **argv) {
	global_set_process(YAMA);
	config = config_leer("metadata");
	log_init(config->log_file, config->log_name, true);

	socket_t sockFS = conectar_con_yamafs(config);
	t_list *nodos = obtener_nodos(sockFS);

	server_crear(config, sockFS, nodos);

	list_destroy_and_destroy_elements(nodos, free);
	server_liberar();
	config_liberar(config);

	return EXIT_SUCCESS;
}
