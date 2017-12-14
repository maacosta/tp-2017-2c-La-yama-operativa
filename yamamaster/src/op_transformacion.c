#include "op_transformacion.h"

t_list *nodos;
sem_t mutex_nodo;
sem_t mutex_yama;

nodos_hilos_t *obtener_nodo(char *ip, char *puerto) {
	nodos_hilos_t *nh = NULL;

	sem_wait(&mutex_nodo);

	int buscar_nodo(nodos_hilos_t *n) {
		return string_equals_ignore_case(&n->ip, ip) && string_equals_ignore_case(&n->puerto, puerto);
	}
	nh = list_find(nodos, (void*)buscar_nodo);
	if(nh == NULL) {
		nh = malloc(sizeof(nodos_hilos_t));
		strcpy(&nh->ip, ip);
		strcpy(&nh->puerto, puerto);
		sem_init(&nh->semaphore, 0, 1);
	}

	sem_post(&mutex_nodo);

	return nh;
}

bool procesar_transformacion(transformacion_hilo_data_t *data, int *numero_job, estado_t *resultado_estado) {
	header_t cabecera;
	packet_t paquete;
	size_t size;

    int num_job;
	char nombre_nodo[NOMBRE_NODO_SIZE];
    char ip[IP_SIZE];
    char puerto[PUERTO_SIZE];
    int num_bloque;
    int bytes_ocupados;
    char nombre_archivo_tmp[NOMBRE_ARCHIVO_TMP];
	serial_string_unpack((char*)data->payload, "h s s s h h s", &num_job, &nombre_nodo, &ip, &puerto, &num_bloque, &bytes_ocupados, &nombre_archivo_tmp);
	free(data->payload);

	*numero_job = num_job;

	log_msg_info("Transformacion: Job [ %d ] Bloque [ %d ] Bytes [ %d ] Nombre tmp [ %s ]", num_job, num_bloque, bytes_ocupados, &nombre_archivo_tmp);

	nodos_hilos_t *nodo = obtener_nodo(&ip, &puerto);

	sem_wait(&nodo->semaphore);

	nodo->socket = conectar_con_worker(&ip, &puerto);
	if(nodo->socket == -1) {
		sem_post(&nodo->semaphore);
		return false;
	}

	//enviar Iniciar Transformacion
	char buffer[BLOQUE_SIZE_E + BYTES_OCUPADOS_SIZE_E + NOMBRE_ARCHIVO_TMP + RESPUESTA_SIZE + 3];
	size = serial_string_pack(&buffer, "h h s h", num_bloque, bytes_ocupados, &nombre_archivo_tmp, data->es_txt_archivo_transformador);
	cabecera = protocol_get_header(OP_WRK_Iniciar_Transformacion, size);
	paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(nodo->socket, &paquete)) {
		socket_close(nodo->socket);
		sem_post(&nodo->semaphore);
		return false;
	}

	//enviar programa Transformacion
	ssize_t size_arc;
	unsigned char *buffer_arc;
	if(data->es_txt_archivo_transformador) buffer_arc = global_read_txtfile(data->nombre_archivo_transformador, &size_arc);
	else buffer_arc = global_read_binfile(data->nombre_archivo_transformador, &size_arc);
	cabecera = protocol_get_header(OP_WRK_Iniciar_Transformacion, size_arc);
	paquete = protocol_get_packet(cabecera, buffer_arc);
	if(!protocol_packet_send(nodo->socket, &paquete)) {
		socket_close(nodo->socket);
		sem_post(&nodo->semaphore);
		return false;
	}
	free(buffer_arc);

	//recibir Iniciar Transformacion
	paquete = protocol_packet_receive(nodo->socket);
	if(paquete.header.operation == OP_ERROR) {
		sem_post(&nodo->semaphore);
		return false;
	}
	resultado_t resultado;
	serial_string_unpack(paquete.payload, "h", &resultado);
	protocol_packet_free(&paquete);

	socket_close(nodo->socket);

	sem_post(&nodo->semaphore);

	sem_wait(&mutex_yama);

	//enviar Estado Transformacion
	char buffer2[NUMERO_JOB_SIZE + RESPUESTA_SIZE + 1];
	size = serial_string_pack(&buffer2, "h h", num_job, resultado);
	cabecera = protocol_get_header(OP_YAM_Enviar_Estado, size);
	paquete = protocol_get_packet(cabecera, &buffer2);
	if(!protocol_packet_send(data->sockYama, &paquete))
		return false;

	//recibir Estado Transformacion
	paquete = protocol_packet_receive(data->sockYama);
	if(paquete.header.operation == OP_ERROR)
		return false;
	estado_t estado;
	serial_string_unpack(paquete.payload, "h", &estado);
	protocol_packet_free(&paquete);

	*resultado_estado = estado;

	sem_post(&mutex_yama);

	return true;
}

void atender_transformacion(transformacion_hilo_data_t *data) {
	estado_t estado;
	int numero_job;
	if(!procesar_transformacion(data, &numero_job, &estado))
		pthread_exit(EXIT_FAILURE);

	if(estado == ESTADO_Error_Replanifica) {
		log_msg_info("Transformacion: Replanifica: Job [ %d ]", numero_job);

		header_t cabecera;
		packet_t paquete;
		size_t size;

		sem_wait(&mutex_yama);

		//enviar Solicitar Transformacion
		char buffer[NUMERO_JOB_SIZE];
		size = serial_string_pack(&buffer, "h", numero_job);
		cabecera = protocol_get_header(OP_YAM_Replanificar_Transformacion, size);
		paquete = protocol_get_packet(cabecera, &buffer);
		if(!protocol_packet_send(data->sockYama, &paquete)) {
			sem_post(&mutex_yama);
			pthread_exit(EXIT_FAILURE);
		}

		paquete = protocol_packet_receive(data->sockYama);
		if(paquete.header.operation == OP_ERROR) {
			sem_post(&mutex_yama);
			pthread_exit(EXIT_FAILURE);
		}

		sem_post(&mutex_yama);

		if(!procesar_transformacion(paquete.payload, &numero_job, &estado))
			pthread_exit(EXIT_FAILURE);
	}

	free(data);

	log_msg_info("Transformacion: Finalizacion [ %s ]: Job [ %d ]", estado == ESTADO_Finalizado_OK ? "OK" : "ERROR", numero_job);

	int r = estado == ESTADO_Finalizado_OK ? EXIT_SUCCESS : EXIT_FAILURE;
	pthread_exit(r);
}

void ejecutar_transformacion(socket_t sockYama, bool es_txt_transformador, char *arc_transformador, char *arc_origen) {
	header_t cabecera;
	packet_t paquete;
	size_t size;

	log_msg_info("Transformacion [ %s ] sobre [ %s ]", arc_transformador, arc_origen);

	//enviar Solicitar Transformacion
	char nombre_archivo[NOMBRE_ARCHIVO];
	size = serial_string_pack(&nombre_archivo, "s", arc_origen);
	cabecera = protocol_get_header(OP_YAM_Solicitar_Transformacion, size);
	paquete = protocol_get_packet(cabecera, &nombre_archivo);
	if(!protocol_packet_send(sockYama, &paquete))
		exit(EXIT_FAILURE);

	//recibir Solicitar Transformacion
	paquete = protocol_packet_receive(sockYama);
	if(paquete.header.operation == OP_ERROR)
		exit(EXIT_FAILURE);
	int i, cant_bloques;
	serial_string_unpack(paquete.payload, "h", &cant_bloques);
	protocol_packet_free(&paquete);

	log_msg_info("Transformacion: Cantidad de bloques [ %d ]", cant_bloques);

	//crear objetos de sincronizacion
	nodos = list_create();
	sem_init(&mutex_nodo, 0, 1);
	sem_init(&mutex_yama, 0, 1);

	pthread_t threads[cant_bloques];
	for(i = 0; i < cant_bloques; i++) {
		paquete = protocol_packet_receive(sockYama);
		if(paquete.header.operation == OP_ERROR)
			exit(EXIT_FAILURE);
		transformacion_hilo_data_t *data = malloc(sizeof(transformacion_hilo_data_t));
		data->sockYama = sockYama;
		data->payload = paquete.payload;
		data->nombre_archivo_transformador = arc_transformador;
		data->es_txt_archivo_transformador = es_txt_transformador;
		threads[i] = thread_create(atender_transformacion, data);
	}
	int r;
	for(i = 0; i < cant_bloques; i++) {
		r = thread_join(threads[i]);
		log_msg_info("Transformacion: Finalizacion de thread [ %d ] exitoso", i);
		if(r == EXIT_FAILURE) {
			log_msg_error("Transformacion: Error en finalizacion de thread [ %d ]", i);
			exit(EXIT_FAILURE);
		}
	}
	list_iterate(nodos, free);
}

