#include "op_almacenamiento.h"

bool almacenamiento_iniciar(packet_t *packet, socket_t sockMaster) {
	int num_job;
	serial_string_unpack(packet->payload, "h", &num_job);
	protocol_packet_free(packet);

	log_msg_info("Etapa Almacenamiento Final: Job [ %d ] Socket Master [ %d ]", num_job, sockMaster);

	header_t cabecera;
	packet_t paquete;
	size_t size;

	int i;
	detalle_nodo_t *nodo;
	estado_master_t *estado_master_nuevo, *estado_master;

	//obtener detalle
	t_list *estados_master = em_obtener_listado(num_job, ETAPA_Reduccion_Global);
	for(i = 0; i < list_size(estados_master); i++) {
		estado_master = list_get(estados_master, i);

		//detalle crear con datos iniciales
		nodo = dn_buscar_por_nodo(&estado_master->nodo);
		break;
	}
	//actualizar lista nodos
	dn_incrementar_carga(nodo);

	//actualizar estados_master
	estado_master_t *em = em_agregar_estado_almacenamiento_final(num_job, &nodo->nodo, sockMaster);

	//enviar Solicitar Almacenamiento Final
	char buffer[NOMBRE_NODO_SIZE + IP_SIZE + PUERTO_SIZE + NOMBRE_ARCHIVO_TMP + 3];
	size = serial_string_pack(&buffer, "s s s s", &em->nodo, &nodo->ip, &nodo->puerto, &estado_master->archivo_temporal);
	cabecera = protocol_get_header(OP_YAM_Solicitar_Almacenamiento_Final, (unsigned long)size);
	paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(sockMaster, &paquete))
		return false;

	return true;
}
