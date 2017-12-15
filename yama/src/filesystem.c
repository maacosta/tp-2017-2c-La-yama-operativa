#include "filesystem.h"

void filesystem_obtener_nodos(socket_t sock) {
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
	serial_string_unpack(paquete.payload, "h", &cant_nodos);
	protocol_packet_free(&paquete);

	log_msg_info("Cantidad de nodos informados [ %d ]", cant_nodos);

	//detalle de cada bloque
	char nodo[NOMBRE_NODO_SIZE];
	char ip[IP_SIZE];
	char puerto[PUERTO_SIZE];
	int i;
	for(i = 0; i < cant_nodos; i++) {
		paquete = protocol_packet_receive(sock);
		if(paquete.header.operation == OP_ERROR)
			exit(EXIT_FAILURE);
		serial_string_unpack(paquete.payload, "s s s", &nodo, &ip, &puerto);
		protocol_packet_free(&paquete);

		dn_agregar_nodo(&nodo, &ip, &puerto);
	}
}
