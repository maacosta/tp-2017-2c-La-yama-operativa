#include "estados_master.h"

int numero_job;
t_list *estados_master;

void em_inicializar() {
	numero_job = 0;
	estados_master = list_create();
}

int em_obtener_proximo_numero_job() {
	return ++numero_job;
}

static void log_estado_master(estado_master_t *estado_master) {
	char nombre[50];
	log_planificador_msg_info("Estado Master Job: %d Socket Master: %d", estado_master->job, estado_master->master);
	log_planificador_msg_info("   #Bloque: %d Nodo: %s Archivo tmp: %s", estado_master->bloque, &estado_master->nodo, &estado_master->archivo_temporal);
	protocol_nombre_etapa(estado_master->etapa, &nombre);
	log_planificador_msg_info("   Etapa: %s", &nombre);
	protocol_nombre_estado(estado_master->estado, &nombre);
	log_planificador_msg_info("   Estado: %s", &nombre);
}

estado_master_t *em_agregar_estado_transformacion(char *nombre_nodo, int num_bloque, socket_t sockMaster) {
	//agregar bloque estado master
	estado_master_t *estado_master = malloc(sizeof(estado_master_t));

	char nombre_arc_tmp[NOMBRE_ARCHIVO_TMP];
	global_nombre_aleatorio("ym_t_", (char*)&nombre_arc_tmp, 6);

	estado_master->job = numero_job;
	estado_master->master = sockMaster;
	strcpy(&estado_master->nodo, nombre_nodo);
	estado_master->bloque = num_bloque;
	estado_master->etapa = ETAPA_Transformacion;
	strcpy(&estado_master->archivo_temporal, &nombre_arc_tmp);
	estado_master->estado = ESTADO_En_Proceso;

	list_add(estados_master, estado_master);

	log_estado_master(estado_master);

	return estado_master;
}

estado_master_t *em_agregar_estado_reduccion(int num_job, char *nombre_nodo, socket_t sockMaster) {
	//agregar bloque estado master
	estado_master_t *estado_master = malloc(sizeof(estado_master_t));

	char nombre_arc_tmp[NOMBRE_ARCHIVO_TMP];
	global_nombre_aleatorio("ym_r_", (char*)&nombre_arc_tmp, 6);

	estado_master->job = num_job;
	estado_master->master = sockMaster;
	strcpy(&estado_master->nodo, nombre_nodo);
	estado_master->bloque = 0;
	estado_master->etapa = ETAPA_Reduccion_Local;
	strcpy(&estado_master->archivo_temporal, &nombre_arc_tmp);
	estado_master->estado = ESTADO_En_Proceso;

	list_add(estados_master, estado_master);

	log_estado_master(estado_master);

	return estado_master;
}

estado_master_t *em_agregar_estado_reduccion_global(int num_job, char *nombre_nodo, socket_t sockMaster) {
	//agregar bloque estado master
	estado_master_t *estado_master = malloc(sizeof(estado_master_t));

	char nombre_arc_tmp[NOMBRE_ARCHIVO_TMP];
	global_nombre_aleatorio("ym_rg_", (char*)&nombre_arc_tmp, 6);

	estado_master->job = num_job;
	estado_master->master = sockMaster;
	strcpy(&estado_master->nodo, nombre_nodo);
	estado_master->bloque = 0;
	estado_master->etapa = ETAPA_Reduccion_Global;
	strcpy(&estado_master->archivo_temporal, &nombre_arc_tmp);
	estado_master->estado = ESTADO_En_Proceso;

	list_add(estados_master, estado_master);

	log_estado_master(estado_master);

	return estado_master;
}

estado_master_t *em_agregar_estado_almacenamiento_final(int num_job, char *nombre_nodo, socket_t sockMaster) {
	//agregar bloque estado master
	estado_master_t *estado_master = malloc(sizeof(estado_master_t));

	estado_master->job = num_job;
	estado_master->master = sockMaster;
	strcpy(&estado_master->nodo, nombre_nodo);
	estado_master->bloque = 0;
	estado_master->etapa = ETAPA_Almacenamiento_Final;
	strcpy(&estado_master->archivo_temporal, "*");
	estado_master->estado = ESTADO_En_Proceso;

	list_add(estados_master, estado_master);

	log_estado_master(estado_master);

	return estado_master;
}

estado_master_t *em_actualizar_estado_bloque(int num_job, char *nombre_nodo, int num_bloque, resultado_t resultado) {
	int buscar_por_job(estado_master_t *em) {
		return em->job == num_job && string_equals_ignore_case(&em->nodo, nombre_nodo) && em->bloque == num_bloque;
	}
	estado_master_t *estado_master = list_find(estados_master, (void *)buscar_por_job);

	if(estado_master == NULL)
		return NULL;

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

	log_estado_master(estado_master);

	return estado_master;
}

t_list *em_obtener_listado(int num_job, etapa_t etapa) {
	bool filtrar_por_job_estado(estado_master_t *estado_master) {
		return estado_master->job == num_job && estado_master->etapa == etapa;
	}
	return list_filter(estados_master, (void*)filtrar_por_job_estado);
}

void em_finalizar() {
	list_destroy_and_destroy_elements(estados_master, (void*)free);
}
