#include "filesystem.h"

t_list *filesystem_obtener_nodos(socket_t sock) {
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

void filesystem_liberar_nodos(t_list *nodos) {
	list_destroy_and_destroy_elements(nodos, free);
}
