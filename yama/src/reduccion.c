#include "reduccion.h"

bool reduccion_iniciar(packet_t *packet, socket_t sockMaster, t_list *estados_master, t_list *nodos) {
	log_msg_info("Etapa Reduccion Local: socket [ %d ]", sockMaster);

	header_t cabecera;
	packet_t paquete;
	size_t size;

	//obtener detalle
	t_list *detalles = list_create();
	detalle_reduccion_t *detalle;
	detalle_nodo_t *nodo;
	char *tmp;
	estado_master_t *estado_master;
	int i;
	for(i = 0; i < list_size(estados_master); i++) {
		estado_master = list_get(estados_master, i);
		if(estado_master->master != sockMaster || estado_master->etapa != ETAPA_Transformacion)
			continue;
		//detalle buscar o crear con datos iniciales
		int buscar_por_nodo(detalle_reduccion_t *d) {
			return string_equals_ignore_case(d->nombre_nodo, estado_master->nodo);
		}
		detalle = list_find(detalle, (void *)buscar_por_nodo);
		if(detalle == NULL) {
			int buscar_por_nodo(detalle_nodo_t *n) {
				return string_equals_ignore_case(n->nodo, estado_master->nodo);
			}
			nodo = list_find(nodos, (void *)buscar_por_nodo);
			detalle = malloc(sizeof(detalle_reduccion_t));
			detalle->numero_job = list_size(estados_master) + 1 + list_size(detalles);
			strcpy(detalle->nombre_nodo, estado_master->nodo);
			strcpy(detalle->ip, nodo->ip);
			strcpy(detalle->puerto, nodo->puerto);
			server_crear_nombre_archivo_temporal(detalle->nombre_archivo_reduccion_local);
			list_add(detalles, detalle);
		}
		//detalle concatenar archivos temporales
		tmp = string_duplicate(detalle->nombre_archivo_temporal);
		string_append(&tmp, ";");
		string_append(&tmp, estado_master->archivo_temporal);
		strcpy(detalle->nombre_archivo_temporal, tmp);
		free(tmp);
	}

	//enviar Solicitar Reduccion
	char buffer[BLOQUE_SIZE_E];
	size = serial_string_pack(&buffer, "h", list_size(detalles));
	cabecera = protocol_get_header(OP_YAM_Solicitar_Reduccion, (unsigned long)size);
	paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(sockMaster, &paquete))
		return false;

	//enviar detalle
	char buffer2[NUMERO_JOB_SIZE + NOMBRE_NODO_SIZE + IP_SIZE + PUERTO_SIZE + NOMBRE_ARCHIVO_TMP*10 + NOMBRE_ARCHIVO_TMP + 5];
	for(i = 0; i < list_size(detalles); i++) {
		detalle = list_get(detalles, i);
		size = serial_string_pack(buffer, "h s s s s s", detalle->numero_job, detalle->nombre_nodo, detalle->ip, detalle->puerto, detalle->nombre_archivo_temporal, detalle->nombre_archivo_reduccion_local);
		cabecera = protocol_get_header(OP_YAM_Solicitar_Reduccion, size);
		paquete = protocol_get_packet(cabecera, &buffer);
		if(!protocol_packet_send(sockMaster, &paquete))
			return false;
		//actualizar lista nodos
		int buscar_por_nodo(detalle_nodo_t *n) {
			return string_equals_ignore_case(n->nodo, detalle->nombre_nodo);
		}
		nodo = list_find(nodos, (void *)buscar_por_nodo);
		nodo->wl += 1;
		nodo->executed_jobs += 1;
		//actualizar estados_master
		estado_master_t *estado_master = malloc(sizeof(estado_master_t));
		estado_master->job = detalle->numero_job;
		estado_master->master = sockMaster;
		strcpy(estado_master->nodo, detalle->nombre_nodo);
		estado_master->bloque = 0;
		estado_master->etapa = ETAPA_Reduccion_Local;
		strcpy(estado_master->archivo_temporal, detalle->nombre_archivo_temporal);
		estado_master->estado = ESTADO_En_Proceso;
		list_add(estados_master, estado_master);
	}
	list_destroy_and_destroy_elements(detalles, free);
	return true;
}

