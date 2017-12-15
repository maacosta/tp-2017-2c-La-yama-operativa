#include "op_jobs_estados_master.h"

bool jem_consultar(packet_t *packet, socket_t sockMaster) {
	estado_master_t *em;
	int num_job, num_bloque;
	char nombre_nodo[NOMBRE_NODO_SIZE];
	resultado_t resultado;
	serial_string_unpack(packet->payload, "h s h h", &num_job, &nombre_nodo, &num_bloque, &resultado);
	protocol_packet_free(packet);

	log_msg_info("Actualizacion de estado: Job [ %d ] Nodo [ %s ] Bloque [ %d ] Resultado [ %s ]", num_job, &nombre_nodo, num_bloque, resultado == RESULTADO_OK ? "OK" : "ERROR");

	if((em = em_actualizar_estado_bloque(num_job, (char*)&nombre_nodo, num_bloque, resultado)) == NULL)
		return false;

	header_t cabecera;
	packet_t paquete;
	size_t size;

	//descontar carga del nodo
	detalle_nodo_t *nodo = dn_buscar_por_nodo(&em->nodo);
	dn_reducir_carga(nodo);

	//enviar Estado
	char buffer[RESPUESTA_SIZE];
	size = serial_string_pack(&buffer, "h", em->estado);
	cabecera = protocol_get_header(OP_YAM_Enviar_Estado, size);
	paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(sockMaster, &paquete))
		return false;

	return true;
}
