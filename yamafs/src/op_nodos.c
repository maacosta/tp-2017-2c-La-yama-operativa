#include "op_nodos.h"


bool nodos_registrar(packet_t *packet, socket_t sockDN, bool *esperarDNs, bool *estadoEstable) {
	datos_nodo_registro_t nodo;
	serial_string_unpack(packet->payload, "s h", &nodo.nombre_nodo, &nodo.cantidad_bloques);
	protocol_packet_free(packet);

	log_msg_info("Registrar nodo [ %d ]", sockDN);

	if(*esperarDNs) {
		;
	}
	else {
		nodo_agregar(&nodo);
		*estadoEstable = true;
	}

	header_t cabecera;
	packet_t paquete;
	size_t size;

	//responder registrar nodo
	char buffer[BLOQUE_SIZE_E];
	size = serial_string_pack(&buffer, "h", (int)RESULTADO_OK);
	cabecera = protocol_get_header(OP_FSY_Registrar_Nodo, size);
	paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(sockDN, &paquete))
		return false;

	return true;
}
