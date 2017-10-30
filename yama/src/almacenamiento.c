#include "almacenamiento.h"

bool almacenamiento_iniciar(packet_t *packet, socket_t sockMaster, t_list *estados_master, t_list *nodos) {
	log_msg_info("Etapa Almacenamiento Final: socket [ %d ]", sockMaster);

	header_t cabecera;
	packet_t paquete;
	size_t size;

	//obtener detalle
	detalle_nodo_t *nodo;
	estado_master_t *estado_master_nuevo, *estado_master;
	int i;
	for(i = 0; i < list_size(estados_master); i++) {
		estado_master = list_get(estados_master, i);
		if(estado_master->master != sockMaster || estado_master->etapa != ETAPA_Reduccion_Global)
			continue;
		//detalle crear con datos iniciales
		int buscar_por_nodo(detalle_nodo_t *n) {
			return string_equals_ignore_case(n->nodo, estado_master->nodo);
		}
		nodo = list_find(nodos, (void *)buscar_por_nodo);
		break;
	}
	//actualizar lista nodos
	nodo->wl += 1;
	nodo->executed_jobs += 1;
	//actualizar estados_master
	estado_master_nuevo = malloc(sizeof(estado_master_t));
	estado_master_nuevo->job = list_size(estados_master) + 1;
	estado_master_nuevo->master = sockMaster;
	strcpy(estado_master_nuevo->nodo, estado_master->nodo);
	estado_master_nuevo->bloque = 0;
	estado_master_nuevo->etapa = ETAPA_Almacenamiento_Final;
	strcpy(estado_master_nuevo->archivo_temporal, estado_master->archivo_temporal);
	estado_master_nuevo->estado = ESTADO_En_Proceso;
	list_add(estados_master, estado_master_nuevo);

	//enviar Solicitar Almacenamiento Final
	char buffer[NUMERO_JOB_SIZE + NOMBRE_NODO_SIZE + IP_SIZE + PUERTO_SIZE + NOMBRE_ARCHIVO_TMP + 4];
	size = serial_string_pack(&buffer, "h s s s s", estado_master_nuevo->job, estado_master_nuevo->nodo, nodo->ip, nodo->puerto, estado_master_nuevo->archivo_temporal);
	cabecera = protocol_get_header(OP_YAM_Solicitar_Almacenamiento_Final, (unsigned long)size);
	paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(sockMaster, &paquete))
		return false;

	return true;
}
