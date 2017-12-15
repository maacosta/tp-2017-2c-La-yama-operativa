#include "detalles_nodo.h"

t_list *nodos;

void dn_inicializar() {
	nodos = list_create();
}

void dn_agregar_nodo(char *nombre_nodo, char *ip, char *puerto) {
	detalle_nodo_t *nodo = malloc(sizeof(detalle_nodo_t));
	strcpy(&nodo->nodo, nombre_nodo);
	strcpy(&nodo->ip, ip);
	strcpy(&nodo->puerto, puerto);
	nodo->wl = 0;
	nodo->executed_jobs = 0;
	list_add(nodos, nodo);

	log_planificador_msg_info("Detalle Nodo: %s Creacion IP: %s PUERTO: %s WL: %d EXECUTED JOBS: %d", &nodo->nodo, &nodo->ip, &nodo->puerto, nodo->wl, nodo->executed_jobs);
}

void dn_incrementar_carga(detalle_nodo_t *nodo) {
	nodo->wl += 1;
	nodo->executed_jobs += 1;

	log_planificador_msg_info("Detalle Nodo: %s WL: %d EXECUTED JOBS: %d", &nodo->nodo, nodo->wl, nodo->executed_jobs);
}

void dn_reducir_carga(detalle_nodo_t *nodo) {
	nodo->wl -= 1;

	log_planificador_msg_info("Detalle Nodo: %s WL: %d EXECUTED JOBS: %d", &nodo->nodo, nodo->wl, nodo->executed_jobs);
}

detalle_nodo_t *dn_buscar_por_nodo(char *nombre_nodo) {
	int buscar_por_nodo(detalle_nodo_t *n) {
		return string_equals_ignore_case(&n->nodo, nombre_nodo);
	}
	return list_find(nodos, (void*)buscar_por_nodo);
}

int dn_obtener_wl_max() {
	unsigned int wlmax = 0;
	int i;
	for(i = 0; i < list_size(nodos); i++) {
		detalle_nodo_t *ndo = list_get(nodos, i);
		if(ndo->wl > wlmax) wlmax = ndo->wl;
	}
	log_planificador_msg_info("WL maxima: %d", wlmax);
	return wlmax;
}

void dn_finalizar() {
	list_destroy_and_destroy_elements(nodos, (void*)free);
}
