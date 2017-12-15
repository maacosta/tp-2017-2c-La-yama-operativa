#include "op_reduccion.h"

bool reduccion_iniciar(packet_t *packet, socket_t sockMaster) {
	int num_job;
	serial_string_unpack(packet->payload, "h", &num_job);
	protocol_packet_free(packet);

	log_msg_info("Etapa Reduccion Local: Job [ %d ] Socket Master [ %d ]", num_job, sockMaster);

	header_t cabecera;
	packet_t paquete;
	size_t size;

	int i;
	char *tmp;
	detalle_reduccion_t *detalle;
	detalle_nodo_t *nodo;
	estado_master_t *estado_master;

	//obtener detalle
	t_list *detalles = list_create();
	t_list *estados_master = em_obtener_listado(num_job, ETAPA_Transformacion);
	for(i = 0; i < list_size(estados_master); i++) {
		estado_master = list_get(estados_master, i);

		//detalle buscar o crear con datos iniciales
		int buscar_por_nodo(detalle_reduccion_t *d) {
			return strcmp(&d->nombre_nodo, &estado_master->nodo) == 0;
		}
		detalle = list_find(detalles, (void*)buscar_por_nodo);

		if(detalle == NULL) {
			nodo = dn_buscar_por_nodo(&estado_master->nodo);

			detalle = malloc(sizeof(detalle_reduccion_t));

			detalle->nombre_archivo_temporal[0] = '\0';
			strcpy(detalle->nombre_nodo, estado_master->nodo);
			strcpy(detalle->ip, nodo->ip);
			strcpy(detalle->puerto, nodo->puerto);

			list_add(detalles, detalle);
		}
		//detalle concatenar archivos temporales
		tmp = string_duplicate(&detalle->nombre_archivo_temporal);
		string_append(&tmp, TOKEN_SEPARADOR_ARCHIVOS);
		string_append(&tmp, &estado_master->archivo_temporal);
		strcpy(&detalle->nombre_archivo_temporal, tmp);
		free(tmp);
	}
	free(estados_master);

	log_msg_info("Etapa Reduccion Local: Cantidad de reducciones [ %d ]", list_size(detalles));

	//enviar Solicitar Reduccion
	char buffer[BLOQUE_SIZE_E];
	size = serial_string_pack(&buffer, "h", list_size(detalles));
	cabecera = protocol_get_header(OP_YAM_Solicitar_Reduccion, (unsigned long)size);
	paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(sockMaster, &paquete))
		return false;

	//enviar detalle
	char buffer2[NUMERO_JOB_SIZE + NOMBRE_NODO_SIZE + IP_SIZE + PUERTO_SIZE + NOMBRE_ARCHIVO_TMP*200 + NOMBRE_ARCHIVO_TMP + 5];
	for(i = 0; i < list_size(detalles); i++) {
		detalle = list_get(detalles, i);

		estado_master = em_agregar_estado_reduccion(num_job, &detalle->nombre_nodo, sockMaster);

		size = serial_string_pack(&buffer2, "h s s s s s", estado_master->job, &estado_master->nodo, &detalle->ip, &detalle->puerto, &detalle->nombre_archivo_temporal, &estado_master->archivo_temporal);
		cabecera = protocol_get_header(OP_YAM_Solicitar_Reduccion, size);
		paquete = protocol_get_packet(cabecera, &buffer2);
		if(!protocol_packet_send(sockMaster, &paquete))
			return false;
		//actualizar lista nodos
		nodo = dn_buscar_por_nodo(&detalle->nombre_nodo);
		dn_incrementar_carga(nodo);
	}
	list_destroy_and_destroy_elements(detalles, free);

	return true;
}

bool reduccion_global_iniciar(packet_t *packet, socket_t sockMaster) {
	int num_job;
	serial_string_unpack(packet->payload, "h", &num_job);
	protocol_packet_free(packet);

	log_msg_info("Etapa Reduccion Global: Job [ %d ] Socket Master [ %d ]", num_job, sockMaster);

	header_t cabecera;
	packet_t paquete;
	size_t size;

	int i, wl_min = INT_MAX;
	detalle_reduccion_global_t *detalle, *detalle_encargado;
	detalle_nodo_t *nodo, *nodo_encargado;
	estado_master_t *estado_master;

	//obtener detalle
	t_list *detalles = list_create();
	t_list *estados_master = em_obtener_listado(num_job, ETAPA_Reduccion_Local);
	for(i = 0; i < list_size(estados_master); i++) {
		estado_master = list_get(estados_master, i);

		//detalle crear con datos iniciales
		nodo = dn_buscar_por_nodo(&estado_master->nodo);

		detalle = malloc(sizeof(detalle_reduccion_global_t));

		strcpy(&detalle->nombre_nodo, &estado_master->nodo);
		strcpy(&detalle->ip, &nodo->ip);
		strcpy(&detalle->puerto, &nodo->puerto);
		strcpy(&detalle->nombre_archivo_temporal, &estado_master->archivo_temporal);

		list_add(detalles, detalle);

		//definir encargado
		if(nodo->wl < wl_min) {
			wl_min = nodo->wl;
			nodo_encargado = nodo;
			detalle_encargado = detalle;
		}
	}

	//actualizar lista nodos
	dn_incrementar_carga(nodo_encargado);

	//actualizar estados_master
	estado_master_t *em = em_agregar_estado_reduccion_global(num_job, &detalle_encargado->nombre_nodo, sockMaster);

	log_msg_info("Etapa Reduccion Global: Cantidad de reducciones [ %d ]", list_size(detalles));

	//enviar Solicitar Reduccion
	char buffer[BLOQUE_SIZE_E + 1];
	size = serial_string_pack((char*)&buffer, "h", list_size(detalles));
	cabecera = protocol_get_header(OP_YAM_Solicitar_Reduccion_Global, (unsigned long)size);
	paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(sockMaster, &paquete))
		return false;

	//enviar detalle
	char buffer2[NOMBRE_NODO_SIZE + IP_SIZE + PUERTO_SIZE + NOMBRE_ARCHIVO_TMP + NOMBRE_ARCHIVO_TMP + RESPUESTA_SIZE + 5];
	for(i = 0; i < list_size(detalles); i++) {
		detalle = list_get(detalles, i);

		if(string_equals_ignore_case(&detalle->nombre_nodo, &em->nodo))
			size = serial_string_pack(&buffer2, "s s s s s h", &detalle->nombre_nodo, &detalle->ip, &detalle->puerto, &detalle->nombre_archivo_temporal, &em->archivo_temporal, true);
		else
			size = serial_string_pack(&buffer2, "s s s s s h", &detalle->nombre_nodo, &detalle->ip, &detalle->puerto, &detalle->nombre_archivo_temporal, "*", false);
		cabecera = protocol_get_header(OP_YAM_Solicitar_Reduccion, size);
		paquete = protocol_get_packet(cabecera, &buffer2);
		if(!protocol_packet_send(sockMaster, &paquete))
			return false;
	}
	list_destroy_and_destroy_elements(detalles, free);
	return true;
}
