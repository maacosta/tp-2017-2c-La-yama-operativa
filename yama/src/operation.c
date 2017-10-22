#include "operation.h"

static unsigned char buffer[4096];
static yama_t* config;
static socket_t sockYAMA;
static socket_t sockFS;
static int global_job_number;

void operation_init(yama_t* c, socket_t sockyama, socket_t sockfs) {
	config = c;
	sockYAMA = sockyama;
	sockFS = sockfs;
}

void aplicar_planificador_de_distribucion(t_list *estados_master, t_list *bloques, socket_t sockMaster) {
	int i, j;
	for(i = 0; i < list_size(bloques); i++) {
		detalle_archivo_t *det = list_get(bloques, i);

		char *stmp = string_new();
		string_append(&stmp, "/tmp/tmp");
		char* snum = string_itoa((global_job_number + 1) * 100 + (i + 1));
		string_append(&stmp, snum);

		estado_master_t *em = malloc(sizeof(estado_master_t));
		em->job = ++global_job_number;
		em->master = sockMaster;
		strcpy(em->nodo, det->nombre_nodo_1);
		em->bloque = det->num_bloque_1;
		em->etapa = ETAPA_Transformacion;
		strcpy(em->archivo_temporal, stmp);
		em->estado = ESTADO_En_Proceso;
		list_add(estados_master, em);

		free(stmp);
		free(snum);
		det->estado_master = em;
	}
}

bool operation_solicitar_transformacion(packet_t *packet, t_list *estados_master, socket_t cliente) {
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
		exit(EXIT_FAILURE);

	//recibir Informacion Archivo
	paquete = protocol_packet_receive(sockFS);
	if(paquete.header.operation == OP_ERROR)
		exit(EXIT_FAILURE);
	int cant_bloques;
	serial_unpack(paquete.payload, "h", &cant_bloques);
	protocol_packet_free(&paquete);

	//detalle de cada bloque
	t_list *bloques = list_create();
	int i;
	for(i = 0; i < cant_bloques; i++) {
		paquete = protocol_packet_receive(sockFS);
		if(paquete.header.operation == OP_ERROR)
			exit(EXIT_FAILURE);
		detalle_archivo_t *det = malloc(sizeof(detalle_archivo_t));
		serial_unpack(paquete.payload, "h s h s h h", det->num_bloque, det->nombre_nodo_1, det->num_bloque_1, det->nombre_nodo_2, det->num_bloque_2, det->tamanio);
		protocol_packet_free(&paquete);
		list_add(bloques, det);
	}

	//determinar nodo segun planificacion configurada
	aplicar_planificador_de_distribucion(estados_master, bloques, cliente);

	//enviar Solicitar Transformacion
	size = serial_string_pack(buffer, "h", cant_bloques);
	cabecera = protocol_get_header(OP_YAM_Solicitar_Transformacion, size);
	paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(cliente, &paquete))
		exit(EXIT_FAILURE);

	//enviar detalle
	for(i = 0; i < cant_bloques; i++) {
		detalle_archivo_t *det = list_get(bloques, i);
		estado_master_t *em = det->estado_master;
		size = serial_string_pack(buffer, "s s s h h s", em->nodo, "127.0.0.1", "5000", em->bloque, det->tamanio, em->archivo_temporal);
		cabecera = protocol_get_header(OP_YAM_Solicitar_Transformacion, size);
		paquete = protocol_get_packet(cabecera, &buffer);
		if(!protocol_packet_send(cliente, &paquete))
			exit(EXIT_FAILURE);
	}

	return true;
}
