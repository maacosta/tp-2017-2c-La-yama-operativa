#include "op_nodos.h"


bool nodos_registrar(packet_t *packet, socket_t sockDN, bool *esperarDNs, bool *estadoEstable) {
	datos_nodo_registro_t nodo;
	serial_string_unpack(packet->payload, "s h", &nodo.nombre_nodo, &nodo.cantidad_bloques);
	protocol_packet_free(packet);

	log_msg_info("Registrar nodo [ %d ]", sockDN);

	if(*esperarDNs) {
		//solo acepta nodos que figuran en los nodos registrados anteriormente
		if(nodo_existe(&nodo))
			nodo_notificar_existencia(&nodo);
		//si todos los nodos estan registrados o los archivos que figuran en el filesystem tienen almenos una copia
		//TODO falta considerar cuando en el filesystem los archivos tienen al menos una copia de los mismos
		if(nodo_todos_registrados() || false)
			*esperarDNs = false;
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
