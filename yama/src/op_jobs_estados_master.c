#include "op_jobs_estados_master.h"

bool jem_consultar(packet_t *packet, socket_t sockMaster, t_list *estados_master, t_list *nodos) {
	int num_job;
	resultado_t resultado;
	serial_string_unpack(packet->payload, "h h", &num_job, &resultado);
	protocol_packet_free(packet);

	log_msg_info("Actualizacion de estado: socket [ %d ]", sockMaster);

	header_t cabecera;
	packet_t paquete;
	size_t size;

	estado_master_t *estado_master;
	int buscar_por_job(estado_master_t *em) {
		return em->job == num_job;
	}
	estado_master = list_find(estados_master, (void *)buscar_por_job);

	if(estado_master->etapa == ETAPA_Transformacion) {
		if(estado_master->estado == ESTADO_En_Proceso) {
			if(resultado == RESULTADO_OK) estado_master->estado = ESTADO_Finalizado_OK;
			if(resultado == RESULTADO_Error) estado_master->estado = ESTADO_Error_Replanifica;
		}
		if(estado_master->estado == ESTADO_Error_Replanifica) {
			if(resultado == RESULTADO_OK) estado_master->estado = ESTADO_Finalizado_OK;
			if(resultado == RESULTADO_Error) estado_master->estado = ESTADO_Error;
		}
	}
	if(estado_master->etapa == ETAPA_Reduccion_Local) {
		if(estado_master->estado == ESTADO_En_Proceso) {
			if(resultado == RESULTADO_OK) estado_master->estado = ESTADO_Finalizado_OK;
			if(resultado == RESULTADO_Error) estado_master->estado = ESTADO_Error;
		}
	}
	if(estado_master->etapa == ETAPA_Reduccion_Global) {
		if(estado_master->estado == ESTADO_En_Proceso) {
			if(resultado == RESULTADO_OK) estado_master->estado = ESTADO_Finalizado_OK;
			if(resultado == RESULTADO_Error) estado_master->estado = ESTADO_Error;
		}
	}
	if(estado_master->etapa == ETAPA_Almacenamiento_Final) {
		if(estado_master->estado == ESTADO_En_Proceso) {
			if(resultado == RESULTADO_OK) estado_master->estado = ESTADO_Finalizado_OK;
			if(resultado == RESULTADO_Error) estado_master->estado = ESTADO_Error;
		}
	}

	//descontar carga del nodo
	detalle_nodo_t *nodo;
	int buscar_por_nodo(detalle_nodo_t *n) {
		return n->nodo == estado_master->nodo;
	}
	nodo = list_find(nodos, (void *)buscar_por_nodo);
	nodo->wl -= 1;

	//enviar Estado
	char buffer[RESPUESTA_SIZE];
	size = serial_string_pack(&buffer, "h", estado_master->estado);
	cabecera = protocol_get_header(OP_YAM_Enviar_Estado, size);
	paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(sockMaster, &paquete))
		return false;

	return true;
}
