#include "op_reduccion.h"

static socket_t sock;
bool es_txt_archivo_reductor;
char *nombre_archivo_reductor;
sem_t mutex_yama;

void atender_reduccion(unsigned char* payload) {
	header_t cabecera;
	packet_t paquete;
	size_t size;

	int num_job;
    char nombre_nodo[NOMBRE_NODO_SIZE];
    char ip[IP_SIZE];
    char puerto[PUERTO_SIZE];
    char nombre_archivos_tmp[NOMBRE_ARCHIVO_TMP*20];
    char nombre_archivo_reduccion_local[NOMBRE_ARCHIVO_TMP];

	serial_string_unpack(payload, "h s s s s s", &num_job, &nombre_nodo, &ip, &puerto, &nombre_archivos_tmp, &nombre_archivo_reduccion_local);
	free(payload);

	socket_t sockWorker = conectar_con_worker(&ip, &puerto);
	if(sockWorker == -1)
		return;

	//enviar Iniciar Reduccion
	char buffer[NOMBRE_ARCHIVO_TMP + NOMBRE_ARCHIVO_TMP*20 + RESPUESTA_SIZE + 2];
	size = serial_string_pack(&buffer, "s s h", &nombre_archivos_tmp, &nombre_archivo_reduccion_local, es_txt_archivo_reductor);
	cabecera = protocol_get_header(OP_WRK_Iniciar_Reduccion, size);
	paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(sockWorker, &paquete)) {
		socket_close(sockWorker);
		pthread_exit(EXIT_FAILURE);
	}

	//enviar programa Reductor
	ssize_t size_arc;
	unsigned char *buffer_arc;
	if(es_txt_archivo_reductor) buffer_arc = global_read_txtfile(nombre_archivo_reductor, &size_arc);
	else buffer_arc = global_read_binfile(nombre_archivo_reductor, &size_arc);
	cabecera = protocol_get_header(OP_WRK_Iniciar_Reduccion, size_arc);
	paquete = protocol_get_packet(cabecera, buffer_arc);
	if(!protocol_packet_send(sockWorker, &paquete)) {
		socket_close(sockWorker);
		pthread_exit(EXIT_FAILURE);
	}
	free(buffer_arc);

	//recibir Iniciar Reduccion
	paquete = protocol_packet_receive(sockWorker);
	if(paquete.header.operation == OP_ERROR) {
		pthread_exit(EXIT_FAILURE);
	}
	resultado_t respuesta;
	serial_string_unpack(paquete.payload, "h", &respuesta);
	protocol_packet_free(&paquete);

	socket_close(sockWorker);

	sem_wait(&mutex_yama);

	//enviar Estado Reduccion
	char buffer2[NUMERO_JOB_SIZE + RESPUESTA_SIZE + 1];
	size = serial_string_pack(&buffer2, "h h", num_job, respuesta);
	cabecera = protocol_get_header(OP_YAM_Enviar_Estado, size);
	paquete = protocol_get_packet(cabecera, &buffer2);
	if(!protocol_packet_send(sock, &paquete)) {
		sem_post(&mutex_yama);
		pthread_exit(EXIT_FAILURE);
	}

	//recibir Estado Reduccion
	paquete = protocol_packet_receive(sock);
	if(paquete.header.operation == OP_ERROR) {
		sem_post(&mutex_yama);
		pthread_exit(EXIT_FAILURE);
	}
	estado_t estado;
	serial_string_unpack(paquete.payload, "h", &estado);
	protocol_packet_free(&paquete);

	sem_post(&mutex_yama);

	log_msg_info("Reduccion: Finalizacion [ %s ]: Job [ %d ]", estado == ESTADO_Finalizado_OK ? "OK" : "ERROR", num_job);

	int r = estado == ESTADO_Finalizado_OK ? EXIT_SUCCESS : EXIT_FAILURE;
	pthread_exit(r);
}

void ejecutar_reduccion(socket_t sockYama, bool es_txt_reductor, char *arc_reductor) {
	sock = sockYama;
	es_txt_archivo_reductor = es_txt_reductor;
	nombre_archivo_reductor = arc_reductor;

	header_t cabecera;
	packet_t paquete;
	size_t size;

	log_msg_info("Reduccion [ %s ]", nombre_archivo_reductor);

	//enviar Solicitar Reduccion
	cabecera = protocol_get_header(OP_YAM_Solicitar_Reduccion, 0);
	paquete = protocol_get_packet(cabecera, NULL);
	if(!protocol_packet_send(sock, &paquete))
		exit(EXIT_FAILURE);

	//recibir Solicitar Reduccion
	paquete = protocol_packet_receive(sock);
	if(paquete.header.operation == OP_ERROR)
		exit(EXIT_FAILURE);
	int i, cant_reducciones;
	serial_string_unpack(paquete.payload, "h", &cant_reducciones);
	protocol_packet_free(&paquete);

	log_msg_info("Reduccion: Cantidad de reducciones [ %d ]", cant_reducciones);

	sem_init(&mutex_yama, 0, 1);

	pthread_t threads[cant_reducciones];
	for(i = 0; i < cant_reducciones; i++) {
		paquete = protocol_packet_receive(sock);
		if(paquete.header.operation == OP_ERROR)
			exit(EXIT_FAILURE);
		threads[i] = thread_create(atender_reduccion, paquete.payload);
	}
	int r;
	for(i = 0; i < cant_reducciones; i++) {
		r = thread_join(threads[i]);
		log_msg_info("Reduccion: Finalizacion de thread [ %d ] exitoso", i);
		if(r == EXIT_FAILURE) {
			log_msg_error("Reduccion: Error en finalizacion de thread [ %d ]", i);
			exit(EXIT_FAILURE);
		}
	}
}

void ejecutar_reduccion_global(socket_t sockYama, bool es_txt_reductor, char *arc_reductor) {
	sock = sockYama;
	es_txt_archivo_reductor = es_txt_reductor;
	nombre_archivo_reductor = arc_reductor;

	header_t cabecera;
	packet_t paquete;
	size_t size;

	log_msg_info("Reduccion Global [ %s ]", nombre_archivo_reductor);

	//enviar Solicitar Reduccion Global
	cabecera = protocol_get_header(OP_YAM_Solicitar_Reduccion_Global, 0);
	paquete = protocol_get_packet(cabecera, NULL);
	if(!protocol_packet_send(sock, &paquete))
		exit(EXIT_FAILURE);

	//recibir Solicitar Reduccion Global
	paquete = protocol_packet_receive(sock);
	if(paquete.header.operation == OP_ERROR)
		exit(EXIT_FAILURE);
	int i, cant_reducciones, num_job;
	serial_string_unpack((char*)paquete.payload, "h h", &num_job, &cant_reducciones);
	protocol_packet_free(&paquete);

	reduccion_global_t reducciones[cant_reducciones];
	int j, i_encargado;
	for(i = 0; i < cant_reducciones; i++) {
		paquete = protocol_packet_receive(sock);
		if(paquete.header.operation == OP_ERROR)
			exit(EXIT_FAILURE);
		serial_string_unpack((char*)paquete.payload, "s s s s s h", &reducciones[i].nombre_nodo, &reducciones[i].ip, &reducciones[i].puerto, &reducciones[i].nombre_archivo_local, &reducciones[i].nombre_archivo_global, &i_encargado);
		reducciones[i].encargado = (bool)i_encargado;
		protocol_packet_free(&paquete);
		if(reducciones[i].encargado)
			j = i;
	}

	socket_t sockWorker = conectar_con_worker(&reducciones[j].ip, &reducciones[j].puerto);
	if(sockWorker == -1)
		return;

	//enviar Iniciar Reduccion Global
	char buffer[NOMBRE_ARCHIVO_TMP + BLOQUE_SIZE_E + RESPUESTA_SIZE + 1];
	size = serial_string_pack(&buffer, "s h h", &reducciones[j].nombre_archivo_global, cant_reducciones, es_txt_reductor);
	cabecera = protocol_get_header(OP_WRK_Iniciar_Reduccion_Global, size);
	paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(sockWorker, &paquete)) {
		socket_close(sockWorker);
		exit(EXIT_FAILURE);
	}

	//enviar programa Reductor
	ssize_t size_arc;
	unsigned char *buffer_arc;
	if(es_txt_reductor) buffer_arc = global_read_txtfile(arc_reductor, &size_arc);
	else buffer_arc = global_read_binfile(arc_reductor, &size_arc);
	cabecera = protocol_get_header(OP_WRK_Iniciar_Reduccion_Global, size_arc);
	paquete = protocol_get_packet(cabecera, buffer_arc);
	if(!protocol_packet_send(sockWorker, &paquete)) {
		socket_close(sockWorker);
		exit(EXIT_FAILURE);
	}
	free(buffer_arc);

	//enviar las reducciones y sus ip y puerto donde estan alojados
	char buffer2[IP_SIZE + PUERTO_SIZE + NOMBRE_ARCHIVO_TMP + 2];
	for(i = 0; i < cant_reducciones; i++) {
		size = serial_string_pack((char*)&buffer2, "s s s", (reducciones[i].encargado ? "*" : &reducciones[i].ip), &reducciones[i].puerto, &reducciones[i].nombre_archivo_local);
		cabecera = protocol_get_header(OP_WRK_Iniciar_Reduccion_Global, size);
		paquete = protocol_get_packet(cabecera, &buffer2);
		if(!protocol_packet_send(sockWorker, &paquete)) {
			socket_close(sockWorker);
			exit(EXIT_FAILURE);
		}
	}
	//recibir Iniciar Reduccion Global
	paquete = protocol_packet_receive(sockWorker);
	if(paquete.header.operation == OP_ERROR) {
		exit(EXIT_FAILURE);
	}
	resultado_t respuesta;
	serial_string_unpack(paquete.payload, "h", &respuesta);
	protocol_packet_free(&paquete);

	//enviar Estado Reduccion Global
	char buffer3[NUMERO_JOB_SIZE + RESPUESTA_SIZE + 1];
	size = serial_string_pack(&buffer3, "h h", num_job, respuesta);
	cabecera = protocol_get_header(OP_YAM_Enviar_Estado, size);
	paquete = protocol_get_packet(cabecera, &buffer3);
	if(!protocol_packet_send(sock, &paquete))
		exit(EXIT_FAILURE);

	//recibir Estado Reduccion Global
	paquete = protocol_packet_receive(sock);
	if(paquete.header.operation == OP_ERROR)
		exit(EXIT_FAILURE);
	estado_t estado;
	serial_string_unpack(paquete.payload, "h", &estado);
	protocol_packet_free(&paquete);

	log_msg_info("Reduccion Global: Finalizacion [ %s ]: Job [ %d ]", estado == ESTADO_Finalizado_OK ? "OK" : "ERROR", num_job);

	if(estado != ESTADO_Finalizado_OK)
		exit(EXIT_FAILURE);
}
