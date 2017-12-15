#include "op_transformacion.h"

char buffer[4096];

planificacion_t *avanzar_clock(yama_t* config, t_list *planificador, planificacion_t *pln_clock, int *pln_clock_index, int pln_size) {
	pln_clock->availability = pln_clock->availability - 1;
	if(++(*pln_clock_index) == pln_size) *pln_clock_index = 0;
	pln_clock = list_get(planificador, *pln_clock_index);
	if(pln_clock->availability == 0) pln_clock->availability = config->disponibilidad_base;
	return pln_clock;
}

void add_bloque_nodo(t_list *bn, char *nodo, int num_bloque, int tamanio) {
	detalle_archivo_seleccionado_t *db = malloc(sizeof(detalle_archivo_seleccionado_t));
	strcpy(&db->nombre_nodo, nodo);
	db->num_bloque = num_bloque;
	db->tamanio = tamanio;
	list_add(bn, db);
	log_planificador_msg_info("Nodo: %s Numero Bloque: %d Tamanio: %d", &db->nombre_nodo, db->num_bloque, db->tamanio);
}

void iterar_planificacion(t_list *bloques_nodo, int pci, int pln_size, t_list *planificador, detalle_archivo_t *det, yama_t *config) {
	int j, pln_clock_index = pci;
	planificacion_t *pln_clock;
	//iterar buscando nodos con disponibilidades (excluir clock)
	for(j = 0; j < pln_size - 1; j++) {
		if(pci + 1 == pln_size) pci = 0; else pci += 1;

		pln_clock = list_get(planificador, pci);
		if(pln_clock->availability == 0) continue;
		if(string_equals_ignore_case(pln_clock->nodo, det->nombre_nodo_1)) {
			add_bloque_nodo(bloques_nodo, &det->nombre_nodo_1, det->num_bloque_1, det->tamanio);
			pln_clock->availability = pln_clock->availability - 1;
			log_planificador_msg_info("Clock Nodo: %s Availability: %d", &pln_clock->nodo, pln_clock->availability);
			return;
		}
		if(string_equals_ignore_case(pln_clock->nodo, det->nombre_nodo_2)) {
			add_bloque_nodo(bloques_nodo, &det->nombre_nodo_2, det->num_bloque_2, det->tamanio);
			pln_clock->availability = pln_clock->availability - 1;
			log_planificador_msg_info("Clock Nodo: %s Availability: %d", &pln_clock->nodo, pln_clock->availability);
			return;
		}
	}

	log_planificador_msg_info("No se encontraron nodos, incrementar disponibilidades");
	//no se encontraron nodos
	//iterar para incrementar las disponibiliades y ahora si designar nodo (incluir clock)
	pci = pln_clock_index;
	bool asignado = false;
	for(j = 0; j < pln_size; j++) {
		if(pci + 1 == pln_size) pci = 0; else pci += 1;

		pln_clock = list_get(planificador, pci);
		pln_clock->availability += config->disponibilidad_base;
		log_planificador_msg_info("Clock Nodo: %s Availability: %d", &pln_clock->nodo, pln_clock->availability);

		if(asignado) continue;
		if(string_equals_ignore_case(pln_clock->nodo, det->nombre_nodo_1)) {
			add_bloque_nodo(bloques_nodo, &det->nombre_nodo_1, det->num_bloque_1, det->tamanio);
			pln_clock->availability = pln_clock->availability - 1;
			log_planificador_msg_info("Clock Nodo: %s Availability: %d", &pln_clock->nodo, pln_clock->availability);
			asignado = true;
		}
		if(string_equals_ignore_case(pln_clock->nodo, det->nombre_nodo_2)) {
			add_bloque_nodo(bloques_nodo, &det->nombre_nodo_2, det->num_bloque_2, det->tamanio);
			pln_clock->availability = pln_clock->availability - 1;
			log_planificador_msg_info("Clock Nodo: %s Availability: %d", &pln_clock->nodo, pln_clock->availability);
			asignado = true;
		}
	}
}

t_list *aplicar_planificacion_de_distribucion(yama_t* config, t_list *bloques) {
	//obtener nodos de los bloques de datos
	log_planificador_msg_info("Disponibilidad de Nodos de los bloques participantes");
	planificacion_t *pln;
	detalle_archivo_t *det;
	t_list *planificador = list_create();
	int i;
	for(i = 0; i < list_size(bloques); i++) {
		det = list_get(bloques, i);

		int buscar_por_nodo_1(planificacion_t *p) {
			return string_equals_ignore_case(p->nodo, det->nombre_nodo_1);
		}
		pln = list_find(planificador, (void *)buscar_por_nodo_1);
		if(pln == NULL) {
			pln = malloc(sizeof(planificacion_t));
			strcpy(pln->nodo, det->nombre_nodo_1);
			pln->availability = config->disponibilidad_base;
			list_add(planificador, pln);
			log_planificador_msg_info("Nodo: %s Availability: %d", &pln->nodo, pln->availability);
		}

		int buscar_por_nodo_2(planificacion_t *p) {
			return string_equals_ignore_case(p->nodo, det->nombre_nodo_2);
		}
		pln = list_find(planificador, (void *)buscar_por_nodo_2);
		if(pln == NULL) {
			pln = malloc(sizeof(planificacion_t));
			strcpy(pln->nodo, det->nombre_nodo_2);
			pln->availability = config->disponibilidad_base;
			list_add(planificador, pln);
			log_planificador_msg_info("Nodo: %s Availability: %d", &pln->nodo, pln->availability);
		}
	}

	//agregar carga a disponibilidad si balanceo es W-CLOCK
	log_planificador_msg_info("Algoritmo de balanceo: %s", config->algoritmo_balanceo);
	detalle_nodo_t *nodo;
	if(string_equals_ignore_case(config->algoritmo_balanceo, "W-CLOCK")) {
		unsigned int wlmax = dn_obtener_wl_max();

		for(i = 0; i < list_size(planificador); i++) {
			pln = list_get(planificador, i);

			nodo = dn_buscar_por_nodo(&pln->nodo);

			pln->availability += wlmax - nodo->wl;
			log_planificador_msg_info("Nodo: %s Availability: %d", &pln->nodo, pln->availability);
		}
	}

	log_planificador_msg_info("Definir Clock");
	//definir clock
	planificacion_t *pln_clock;
	int pln_size = list_size(planificador);
	int pln_clock_index = 0;
	for(i = 0; i < pln_size; i++) {
		pln = list_get(planificador, i);

		if(i == 0) pln_clock = pln;
		else if(pln->availability > pln_clock->availability) {
			pln_clock = pln;
			pln_clock_index = i;
		}
		else if(pln->availability == pln_clock->availability) {
			nodo = dn_buscar_por_nodo(&pln->nodo);
			int ejs = nodo->executed_jobs;

			nodo = dn_buscar_por_nodo(&pln_clock->nodo);
			int ejsC = nodo->executed_jobs;

			if(ejs > ejsC) {
				pln_clock = pln;
				pln_clock_index = i;
			}
		}
	}
	log_planificador_msg_info("Clock Nodo: %s Index: %d", pln_clock->nodo, pln_clock_index);

	//aplicar balanceo de carga
	log_planificador_msg_info("Aplicacion de balanceo en los nodos y bloques");
	t_list *bloques_nodo = list_create();
	detalle_archivo_seleccionado_t *det_sel;
	for(i = 0; i < list_size(bloques); i++) {
		det = list_get(bloques, i);
		//si nodo del bloque coincide con nodo del clock
		if(string_equals_ignore_case(pln_clock->nodo, det->nombre_nodo_1)) {
			add_bloque_nodo(bloques_nodo, &det->nombre_nodo_1, det->num_bloque_1, det->tamanio);
			pln_clock = avanzar_clock(config, planificador, pln_clock, &pln_clock_index, pln_size);
			continue;
		}
		if(string_equals_ignore_case(pln_clock->nodo, det->nombre_nodo_2)) {
			add_bloque_nodo(bloques_nodo, &det->nombre_nodo_2, det->num_bloque_2, det->tamanio);
			pln_clock = avanzar_clock(config, planificador, pln_clock, &pln_clock_index, pln_size);
			continue;
		}
		//si nodo del bloque no coincide con clock, iterar
		iterar_planificacion(bloques_nodo, pln_clock_index, pln_size, planificador, det, config);
	}

	list_destroy_and_destroy_elements(planificador, free);

	return bloques_nodo;
}

bool transformacion_iniciar(packet_t *packet, socket_t sockMaster, socket_t sockFS, yama_t* config) {
	char archivo_a_procesar[NOMBRE_ARCHIVO];
	serial_string_unpack(packet->payload, "s", &archivo_a_procesar);
	protocol_packet_free(packet);

	log_msg_info("Etapa Transformacion: Socket Master [ %d ] Archivo [ %s ]", sockMaster, &archivo_a_procesar);

	header_t cabecera;
	packet_t paquete;
	size_t size;

	//enviar consulta Informacion Archivo
	cabecera = protocol_get_header(OP_FSY_Informacion_Archivo, packet->header.size);
	paquete = protocol_get_packet(cabecera, &archivo_a_procesar);
	if(!protocol_packet_send(sockFS, &paquete))
		return false;
	//recibir Informacion Archivo
	paquete = protocol_packet_receive(sockFS);
	if(paquete.header.operation == OP_ERROR)
		return false;
	int cant_bloques;
	serial_string_unpack(paquete.payload, "h", &cant_bloques);
	protocol_packet_free(&paquete);

	log_msg_info("Archivo a transformar con [ %d ] bloques", cant_bloques);

	//obtener detalle de cada bloque
	t_list *bloques = list_create();
	int i;
	for(i = 0; i < cant_bloques; i++) {
		paquete = protocol_packet_receive(sockFS);
		if(paquete.header.operation == OP_ERROR)
			return false;
		detalle_archivo_t *det = malloc(sizeof(detalle_archivo_t));
		serial_string_unpack(paquete.payload, "h s h s h h", &det->num_bloque, &det->nombre_nodo_1, &det->num_bloque_1, &det->nombre_nodo_2, &det->num_bloque_2, &det->tamanio);
		protocol_packet_free(&paquete);
		list_add(bloques, det);
	}

	//determinar nodo segun planificacion configurada
	t_list *bloques_nodo = aplicar_planificacion_de_distribucion(config, bloques);
	list_destroy_and_destroy_elements(bloques, free);

	//enviar Solicitar Transformacion
	int num_job = em_obtener_proximo_numero_job();
	size = serial_string_pack(&buffer, "h h", num_job, cant_bloques);
	cabecera = protocol_get_header(OP_YAM_Solicitar_Transformacion, (unsigned long)size);
	paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(sockMaster, &paquete))
		return false;

	//enviar detalle
	detalle_archivo_seleccionado_t *det_sel;
	detalle_nodo_t *nodo;
	for(i = 0; i < cant_bloques; i++) {
		det_sel = list_get(bloques_nodo, i);

		nodo = dn_buscar_por_nodo(&det_sel->nombre_nodo);

		estado_master_t *em = em_agregar_estado_transformacion((char*)&det_sel->nombre_nodo, det_sel->num_bloque, sockMaster);

		size = serial_string_pack(&buffer, "h s s s h h s", em->job, &em->nodo, &nodo->ip, &nodo->puerto, em->bloque, det_sel->tamanio, &em->archivo_temporal);
		cabecera = protocol_get_header(OP_YAM_Solicitar_Transformacion, size);
		paquete = protocol_get_packet(cabecera, &buffer);
		if(!protocol_packet_send(sockMaster, &paquete))
			return false;
		//actualizar lista nodos
		dn_incrementar_carga(nodo);
	}
	list_destroy_and_destroy_elements(bloques_nodo, free);
	return true;
}
