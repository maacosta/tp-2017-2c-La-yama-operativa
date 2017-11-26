#include "op_nodos.h"


bool nodos_registrar(packet_t *packet, socket_t sockDN, yamafs_t *config, bool *esperarDNs, bool *estadoEstable) {
	datos_nodo_registro_t nodo;
	serial_string_unpack(packet->payload, "s h", &nodo.nombre_nodo, &nodo.cantidad_bloques);
	protocol_packet_free(packet);

	log_msg_info("Registracion de nodo [ %s ]", nodo.nombre_nodo);

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

		bitmap_t bm = bitmap_crear(config, &nodo.nombre_nodo, nodo.cantidad_bloques);
		bitmap_destruir(&bm);

		*estadoEstable = true;
	}
	return true;
}
