#include "op_reduccion.h"

sem_t mutex_yama;

void atender_reduccion(reduccion_hilo_data_t *data) {
	header_t cabecera;
	packet_t paquete;
	size_t size;

	int num_job;
    char nombre_nodo[NOMBRE_NODO_SIZE];
    char ip[IP_SIZE];
    char puerto[PUERTO_SIZE];
    char nombre_archivos_tmp[NOMBRE_ARCHIVO_TMP*200];
    char nombre_archivo_reduccion_local[NOMBRE_ARCHIVO_TMP];

	serial_string_unpack(data->payload, "h s s s s s", &num_job, &nombre_nodo, &ip, &puerto, &nombre_archivos_tmp, &nombre_archivo_reduccion_local);
	free(data->payload);

	socket_t sockWorker = conectar_con_worker(&ip, &puerto);
	if(sockWorker == -1)
		return;

	//enviar Iniciar Reduccion
	char buffer[NOMBRE_ARCHIVO_TMP + NOMBRE_ARCHIVO_TMP*200 + RESPUESTA_SIZE + 2];
	size = serial_string_pack(&buffer, "s s h", &nombre_archivos_tmp, &nombre_archivo_reduccion_local, data->es_txt_archivo_reductor);
	cabecera = protocol_get_header(OP_WRK_Iniciar_Reduccion, size);
	paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(sockWorker, &paquete)) {
		socket_close(sockWorker);
		pthread_exit(EXIT_FAILURE);
	}

	//enviar programa Reductor
	ssize_t size_arc;
	unsigned char *buffer_arc;
	if(data->es_txt_archivo_reductor) buffer_arc = global_read_txtfile(data->nombre_archivo_reductor, &size_arc);
	else buffer_arc = global_read_binfile(data->nombre_archivo_reductor, &size_arc);
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
	char buffer2[NUMERO_JOB_SIZE + NOMBRE_NODO_SIZE + BLOQUE_SIZE_E + RESPUESTA_SIZE + 3];
	size = serial_string_pack(&buffer2, "h s h h", num_job, &nombre_nodo, 0, respuesta);
	cabecera = protocol_get_header(OP_YAM_Enviar_Estado, size);
	paquete = protocol_get_packet(cabecera, &buffer2);
	if(!protocol_packet_send(data->sockYama, &paquete)) {
		sem_post(&mutex_yama);
		pthread_exit(EXIT_FAILURE);
	}

	//recibir Estado Reduccion
	paquete = protocol_packet_receive(data->sockYama);
	if(paquete.header.operation == OP_ERROR) {
		sem_post(&mutex_yama);
		pthread_exit(EXIT_FAILURE);
	}
	estado_t estado;
	serial_string_unpack(paquete.payload, "h", &estado);
	protocol_packet_free(&paquete);

	sem_post(&mutex_yama);

	free(data);

	log_msg_info("Reduccion: Finalizacion [ %s ]: Job [ %d ]", estado == ESTADO_Finalizado_OK ? "OK" : "ERROR", num_job);

	int r = estado == ESTADO_Finalizado_OK ? EXIT_SUCCESS : EXIT_FAILURE;
	pthread_exit(r);
}

void ejecutar_reduccion(socket_t sockYama, bool es_txt_reductor, char *arc_reductor, int num_job) {
	header_t cabecera;
	packet_t paquete;
	size_t size;

	log_msg_info("Reduccion: Job [ %d ] [ %s ]", num_job, arc_reductor);

	//enviar Solicitar Reduccion
	char buffer[NUMERO_JOB_SIZE];
	size = serial_string_pack(&buffer, "h", num_job);
	cabecera = protocol_get_header(OP_YAM_Solicitar_Reduccion, size);
	paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(sockYama, &paquete))
		exit(EXIT_FAILURE);

	//recibir Solicitar Reduccion
	paquete = protocol_packet_receive(sockYama);
	if(paquete.header.operation == OP_ERROR)
		exit(EXIT_FAILURE);
	int i, cant_reducciones;
	serial_string_unpack(paquete.payload, "h", &cant_reducciones);
	protocol_packet_free(&paquete);

	log_msg_info("Reduccion: Cantidad de reducciones [ %d ]", cant_reducciones);

	sem_init(&mutex_yama, 0, 1);

	pthread_t threads[cant_reducciones];
	for(i = 0; i < cant_reducciones; i++) {
		paquete = protocol_packet_receive(sockYama);
		if(paquete.header.operation == OP_ERROR)
			exit(EXIT_FAILURE);
		reduccion_hilo_data_t *data = malloc(sizeof(reduccion_hilo_data_t));
		data->sockYama = sockYama;
		data->payload = paquete.payload;
		data->nombre_archivo_reductor = arc_reductor;
		data->es_txt_archivo_reductor = es_txt_reductor;
		threads[i] = thread_create(atender_reduccion, data);
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

void ejecutar_reduccion_global(socket_t sockYama, bool es_txt_reductor, char *arc_reductor, int num_job) {
	header_t cabecera;
	packet_t paquete;
	size_t size;

	log_msg_info("Reduccion Global: Job [ %d ] [ %s ]", num_job, arc_reductor);

	//enviar Solicitar Reduccion Global
	char buffer[NUMERO_JOB_SIZE];
	size = serial_string_pack(&buffer, "h", num_job);
	cabecera = protocol_get_header(OP_YAM_Solicitar_Reduccion_Global, size);
	paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(sockYama, &paquete))
		exit(EXIT_FAILURE);

	//recibir Solicitar Reduccion Global
	paquete = protocol_packet_receive(sockYama);
	if(paquete.header.operation == OP_ERROR)
		exit(EXIT_FAILURE);
	int i, cant_reducciones;
	serial_string_unpack((char*)paquete.payload, "h", &cant_reducciones);
	protocol_packet_free(&paquete);

	reduccion_global_t reducciones[cant_reducciones];
	int j, i_encargado;
	for(i = 0; i < cant_reducciones; i++) {
		paquete = protocol_packet_receive(sockYama);
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
	char buffer2[NOMBRE_ARCHIVO_TMP + BLOQUE_SIZE_E + RESPUESTA_SIZE + 1];
	size = serial_string_pack(&buffer2, "s h h", &reducciones[j].nombre_archivo_global, cant_reducciones, es_txt_reductor);
	cabecera = protocol_get_header(OP_WRK_Iniciar_Reduccion_Global, size);
	paquete = protocol_get_packet(cabecera, &buffer2);
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
	char buffer3[IP_SIZE + PUERTO_SIZE + NOMBRE_ARCHIVO_TMP + 2];
	for(i = 0; i < cant_reducciones; i++) {
		if(reducciones[i].encargado)
			size = serial_string_pack((char*)&buffer3, "s s s", "*", &reducciones[i].puerto, &reducciones[i].nombre_archivo_local);
		else
			size = serial_string_pack((char*)&buffer3, "s s s", &reducciones[i].ip, &reducciones[i].puerto, &reducciones[i].nombre_archivo_local);
		cabecera = protocol_get_header(OP_WRK_Iniciar_Reduccion_Global, size);
		paquete = protocol_get_packet(cabecera, &buffer3);
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

	socket_close(sockWorker);

	//enviar Estado Reduccion Global
	char buffer4[NUMERO_JOB_SIZE + NOMBRE_NODO_SIZE + BLOQUE_SIZE_E + RESPUESTA_SIZE + 3];
	size = serial_string_pack(&buffer4, "h s h h", num_job, &reducciones[j].nombre_nodo, 0, respuesta);
	cabecera = protocol_get_header(OP_YAM_Enviar_Estado, size);
	paquete = protocol_get_packet(cabecera, &buffer4);
	if(!protocol_packet_send(sockYama, &paquete))
		exit(EXIT_FAILURE);

	//recibir Estado Reduccion Global
	paquete = protocol_packet_receive(sockYama);
	if(paquete.header.operation == OP_ERROR)
		exit(EXIT_FAILURE);
	estado_t estado;
	serial_string_unpack(paquete.payload, "h", &estado);
	protocol_packet_free(&paquete);

	log_msg_info("Reduccion Global: Finalizacion [ %s ]: Job [ %d ]", estado == ESTADO_Finalizado_OK ? "OK" : "ERROR", num_job);

	if(estado != ESTADO_Finalizado_OK)
		exit(EXIT_FAILURE);
}
