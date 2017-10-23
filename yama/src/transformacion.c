#include "transformacion.h"

char buffer[4096];

int avanzar_clock(yama_t* config, t_list *planificador, planificacion_t *pln_clock, int pln_clock_index, int pln_size) {
	pln_clock->availability = pln_clock->availability - 1;
	if(pln_clock_index + 1 == pln_size) pln_clock_index = 0;
	pln_clock = list_get(planificador, pln_clock_index);
	if(pln_clock->availability == 0) pln_clock->availability = config->disponibilidad_base;
	return pln_clock_index;
}

void iterar_planificacion(detalle_archivo_seleccionado_t *det_sel, int pci, int pln_size, t_list *planificador, detalle_archivo_t *det, yama_t *config) {
	int j, pln_clock_index = pci;
	planificacion_t *pln_clock;
	//iterar buscando nodos con disponibilidades (excluir clock)
	for(j = 0; j < pln_size - 1; j++) {
		if(pci + 1 == pln_size) pci = 0; else pci += 1;
		pln_clock = list_get(planificador, pci);
		if(pln_clock->availability == 0) continue;
		if(string_equals_ignore_case(pln_clock->nodo, det->nombre_nodo_1)) {
			strcpy(det_sel->nombre_nodo, det->nombre_nodo_1);
			det_sel->num_bloque = det->num_bloque_1;
			det_sel->tamanio = det->tamanio;
			pln_clock->availability = pln_clock->availability - 1;
			return;
		}
		if(string_equals_ignore_case(pln_clock->nodo, det->nombre_nodo_2)) {
			strcpy(det_sel->nombre_nodo, det->nombre_nodo_2);
			det_sel->num_bloque = det->num_bloque_2;
			det_sel->tamanio = det->tamanio;
			pln_clock->availability = pln_clock->availability - 1;
			return;
		}
	}
	//no se encontraron nodos
	//iterar para incrementar las disponibiliades y ahora si designar nodo (incluir clock)
	pci = pln_clock_index;
	bool asignado = false;
	for(j = 0; j < pln_size; j++) {
		if(pci + 1 == pln_size) pci = 0; else pci += 1;
		pln_clock = list_get(planificador, pci);
		pln_clock->availability += config->disponibilidad_base;
		if(asignado) continue;
		if(string_equals_ignore_case(pln_clock->nodo, det->nombre_nodo_1)) {
			strcpy(det_sel->nombre_nodo, det->nombre_nodo_1);
			det_sel->num_bloque = det->num_bloque_1;
			det_sel->tamanio = det->tamanio;
			pln_clock->availability = pln_clock->availability - 1;
			asignado = true;
		}
		if(string_equals_ignore_case(pln_clock->nodo, det->nombre_nodo_2)) {
			strcpy(det_sel->nombre_nodo, det->nombre_nodo_2);
			det_sel->num_bloque = det->num_bloque_2;
			det_sel->tamanio = det->tamanio;
			pln_clock->availability = pln_clock->availability - 1;
			asignado = true;
		}
	}
}

t_list *aplicar_planificacion_de_distribucion(yama_t* config, t_list *estados_master, t_list *bloques, t_list *nodos) {
	log_msg_info("Etapa Transformacion: Planificacion: Balanceo [ %s ]", config->algoritmo_balanceo);

	//obtener nodos de los bloques de datos
	planificacion_t *pln;
	detalle_archivo_t *det;
	t_list *planificador = list_create();
	int i;
	for(i = 0; i < list_size(bloques); i++) {
		det = list_get(bloques, i);
		int buscar_por_nodo_1(planificacion_t *p) {
			return string_equals_ignore_case(p->nodo, det->nombre_nodo_1);
		}
		int buscar_por_nodo_2(planificacion_t *p) {
			return string_equals_ignore_case(p->nodo, det->nombre_nodo_2);
		}
		pln = list_find(planificador, (void *)buscar_por_nodo_1);
		if(pln == NULL) {
			pln = malloc(sizeof(planificacion_t));
			strcpy(pln->nodo, det->nombre_nodo_1);
			pln->availability = config->disponibilidad_base;
			list_add(planificador, pln);
		}
		pln = list_find(planificador, (void *)buscar_por_nodo_2);
		if(pln == NULL) {
			pln = malloc(sizeof(planificacion_t));
			strcpy(pln->nodo, det->nombre_nodo_2);
			pln->availability = config->disponibilidad_base;
			list_add(planificador, pln);
		}
	}
	//agregar carga a disponibilidad si balanceo es W-CLOCK
	detalle_nodo_t *ndo;
	if(string_equals_ignore_case(config->algoritmo_balanceo, "W-CLOCK")) {
		log_msg_info("Etapa Transformacion: Planificacion: Calculo de carga segun balanceo [ %s ]", config->algoritmo_balanceo);
		unsigned int wlmax = 0;
		for(i = 0; i < list_size(nodos); i++) {
			ndo = list_get(nodos, i);
			if(ndo->wl > wlmax) wlmax = ndo->wl;
		}
		for(i = 0; i < list_size(planificador); i++) {
			pln = list_get(planificador, i);
			int buscar_por_nodo(detalle_nodo_t *n) {
				return string_equals_ignore_case(n->nodo, pln->nodo);
			}
			ndo = list_find(nodos, (void *)buscar_por_nodo);
			pln->availability += wlmax - ndo->wl;
		}
	}
	log_msg_info("Etapa Transformacion: Planificacion: Definir Clock");
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
			int buscar_por_nodo(detalle_nodo_t *n) {
				return string_equals_ignore_case(n->nodo, pln->nodo);
			}
			int buscar_por_nodo_clock(detalle_nodo_t *n) {
				return string_equals_ignore_case(n->nodo, pln_clock->nodo);
			}
			ndo = list_find(nodos, (void *)buscar_por_nodo);
			int ejs = ndo->executed_jobs;
			ndo = list_find(nodos, (void *)buscar_por_nodo_clock);
			int ejsC = ndo->executed_jobs;
			if(ejs > ejsC) {
				pln_clock = pln;
				pln_clock_index = i;
			}
		}
	}
	log_msg_info("Etapa Transformacion: Planificacion: Aplicar balanceo");
	//aplicar balanceo de carga
	t_list *bloques_nodo = list_create();
	detalle_archivo_seleccionado_t *det_sel;
	for(i = 0; i < list_size(bloques); i++) {
		det = list_get(bloques, i);
		det_sel = malloc(sizeof(detalle_archivo_seleccionado_t));
		//si nodo del bloque coincide con nodo del clock
		if(string_equals_ignore_case(pln_clock->nodo, det->nombre_nodo_1)) {
			strcpy(det_sel->nombre_nodo, det->nombre_nodo_1);
			det_sel->num_bloque = det->num_bloque_1;
			det_sel->tamanio = det->tamanio;
			pln_clock_index = avanzar_clock(config, planificador, &pln_clock, pln_clock_index, pln_size);
			continue;
		}
		if(string_equals_ignore_case(pln_clock->nodo, det->nombre_nodo_2)) {
			strcpy(det_sel->nombre_nodo, det->nombre_nodo_2);
			det_sel->num_bloque = det->num_bloque_2;
			det_sel->tamanio = det->tamanio;
			pln_clock_index = avanzar_clock(config, planificador, &pln_clock, pln_clock_index, pln_size);
			continue;
		}
		//si nodo del bloque no coincide con clock, iterar
		iterar_planificacion(det_sel, pln_clock_index, pln_size, planificador, det, config);
		//agregar bloque de nodo seleccionado a la lista a retornar
		list_add(bloques_nodo, det_sel);
	}

	list_destroy_and_destroy_elements(planificador, free);

	return bloques_nodo;
}

bool transformacion_iniciar(packet_t *packet, socket_t sockMaster, socket_t sockFS, yama_t* config, t_list *estados_master, t_list *nodos) {
	char archivo_a_procesar[NOMBRE_ARCHIVO];
	serial_string_unpack(packet->payload, "s", &archivo_a_procesar);

	log_msg_info("Etapa Transformacion: archivo [ %s ]", archivo_a_procesar);

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
	serial_unpack(paquete.payload, "h", &cant_bloques);
	protocol_packet_free(&paquete);
	//obtener detalle de cada bloque
	t_list *bloques = list_create();
	int i;
	for(i = 0; i < cant_bloques; i++) {
		paquete = protocol_packet_receive(sockFS);
		if(paquete.header.operation == OP_ERROR)
			return false;
		detalle_archivo_t *det = malloc(sizeof(detalle_archivo_t));
		serial_unpack(paquete.payload, "h s h s h h", det->num_bloque, det->nombre_nodo_1, det->num_bloque_1, det->nombre_nodo_2, det->num_bloque_2, det->tamanio);
		protocol_packet_free(&paquete);
		list_add(bloques, det);
	}

	//determinar nodo segun planificacion configurada
	t_list *bloques_nodo = aplicar_planificacion_de_distribucion(config, estados_master, bloques, nodos);
	list_destroy_and_destroy_elements(bloques, free);

	//enviar Solicitar Transformacion
	size = serial_string_pack(&buffer, "h", cant_bloques);
	cabecera = protocol_get_header(OP_YAM_Solicitar_Transformacion, (unsigned long)size);
	paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(sockMaster, &paquete))
		return false;

	//enviar detalle
	detalle_archivo_seleccionado_t *det_sel;
	detalle_nodo_t *nodo;
	for(i = 0; i < cant_bloques; i++) {
		det_sel = list_get(bloques_nodo, i);
		int buscar_por_nodo(detalle_nodo_t *n) {
			return string_equals_ignore_case(n->nodo, det_sel->nombre_nodo);
		}
		nodo = list_find(nodos, (void *)buscar_por_nodo);
		size = serial_string_pack(buffer, "s s s h h s", det_sel->nombre_nodo, nodo->ip, nodo->puerto, det_sel->num_bloque, det_sel->tamanio, "nombre archivo temporal");
		cabecera = protocol_get_header(OP_YAM_Solicitar_Transformacion, size);
		paquete = protocol_get_packet(cabecera, &buffer);
		if(!protocol_packet_send(sockMaster, &paquete))
			return false;
	}
	list_destroy_and_destroy_elements(bloques_nodo, free);
	return true;
}
