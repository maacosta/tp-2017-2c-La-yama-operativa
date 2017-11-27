#include "op_nodos.h"

t_list *lista_nodos;

bool nodos_registrar(packet_t *packet, socket_t sockDN, yamafs_t *config, bool *esperarDNs, bool *estadoEstable) {
	nodo_detalle_t *nodo = malloc(sizeof(nodo_detalle_t));
	serial_string_unpack(packet->payload, "s h s s", nodo->nombre_nodo, nodo->cant_bloques, nodo->ip, nodo->puerto);
	protocol_packet_free(packet);

	log_msg_info("op_nodos | Registracion de nodo [ %s ]", nodo->nombre_nodo);

	if(lista_nodos == NULL) lista_nodos = list_create();

	if(*esperarDNs) {
		//solo acepta nodos que figuran en los nodos registrados anteriormente
		if(nodo_existe(nodo->nombre_nodo)) {
			nodo_notificar_existencia(nodo->nombre_nodo);
			list_add(lista_nodos, nodo);
		}
		else {
			free(nodo);
			log_msg_error("op_nodos | Se cierra conexion de un nodo que no existe en el estado anterior");
			return false;
		}

		//es estado-estable cuando se conectaron todos los nodos del config o
		//cuando los archivos en el sistema tienen al menos una copia vigente en los nodos conectados
		if(nodo_se_notificaron_todos_los_registrados()) {
			*esperarDNs = false;
			*estadoEstable = true;
		}
	}
	else {
		nodo_agregar(nodo->nombre_nodo, nodo->cant_bloques, nodo->cant_bloques);
		list_add(lista_nodos, nodo);
		bitmap_t bm = bitmap_crear(config, nodo->nombre_nodo, nodo->cant_bloques);
		bitmap_destruir(&bm);

		//Con un solo nodo conectado ya es estado-estable
		*estadoEstable = true;
	}
	return true;
}

bool nodos_informar(packet_t *packet, socket_t sockYAMA) {
	protocol_packet_free(packet);

	log_msg_info("Informar nodos [ %d ]", sockYAMA);

	header_t cabecera;
	packet_t paquete;
	size_t size;

	//enviar cantidad de Nodos
	char buffer[BLOQUE_SIZE_E];
	size = serial_string_pack(&buffer, "h", list_size(lista_nodos));
	cabecera = protocol_get_header(OP_FSY_Obtener_Nodos, size);
	paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(sockYAMA, &paquete))
		return false;

	bool resultado = true;
	void iterar(nodo_detalle_t *n) {
		if(!resultado) return;
		//enviar detalle de Nodo
		char buffer2[NOMBRE_NODO_SIZE + IP_SIZE + PUERTO_SIZE + 2];
		size = serial_string_pack(&buffer2, "s s s", n->nombre_nodo, n->ip, n->puerto);
		cabecera = protocol_get_header(OP_FSY_Obtener_Nodos, size);
		paquete = protocol_get_packet(cabecera, &buffer2);
		if(!protocol_packet_send(sockYAMA, &paquete))
			resultado = false;
	}
	list_iterate(lista_nodos, (void *)iterar);

	return resultado;
}
