#include "op_transformacion.h"

static socket_t sock;
bool es_txt_archivo_transformador;
char *nombre_archivo_transformador;

bool procesar_transformacion(unsigned char* payload, int *numero_job, estado_t *resultado_estado) {
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
	serial_string_unpack((char *)payload, "h s s s h h s", &num_job, &nombre_nodo, &ip, &puerto, &num_bloque, &bytes_ocupados, &nombre_archivo_tmp);
	free(payload);

	*numero_job = num_job;

	socket_t sockWorker = conectar_con_worker(&ip, &puerto);
	if(sockWorker == -1)
		return false;

	//enviar Iniciar Transformacion
	char buffer[BLOQUE_SIZE_E + BYTES_OCUPADOS_SIZE_E + NOMBRE_ARCHIVO_TMP + RESPUESTA_SIZE + 3];
	size = serial_string_pack(&buffer, "h h s h", num_bloque, bytes_ocupados, &nombre_archivo_tmp, es_txt_archivo_transformador);
	cabecera = protocol_get_header(OP_WRK_Iniciar_Transformacion, size);
	paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(sockWorker, &paquete))
		return false;

	//enviar programa Transformacion
	ssize_t size_arc;
	unsigned char *buffer_arc;
	if(es_txt_archivo_transformador) buffer_arc = global_read_txtfile(nombre_archivo_transformador, &size_arc);
	else buffer_arc = global_read_binfile(nombre_archivo_transformador, &size_arc);
	cabecera = protocol_get_header(OP_WRK_Iniciar_Transformacion, size_arc);
	paquete = protocol_get_packet(cabecera, buffer_arc);
	if(!protocol_packet_send(sockWorker, &paquete))
		return false;
	free(buffer_arc);

	//recibir Iniciar Transformacion
	paquete = protocol_packet_receive(sockWorker);
	if(paquete.header.operation == OP_ERROR)
		return false;
	resultado_t resultado;
	serial_string_unpack(paquete.payload, "h", &resultado);
	protocol_packet_free(&paquete);

	//enviar Estado Transformacion
	char buffer2[NUMERO_JOB_SIZE + RESPUESTA_SIZE + 1];
	size = serial_string_pack(buffer2, "h h", num_job, resultado);
	cabecera = protocol_get_header(OP_YAM_Enviar_Estado, size);
	paquete = protocol_get_packet(cabecera, &buffer2);
	if(!protocol_packet_send(sock, &paquete))
		return false;

	//recibir Estado Transformacion
	paquete = protocol_packet_receive(sock);
	if(paquete.header.operation == OP_ERROR)
		return false;
	estado_t estado;
	serial_string_unpack(paquete.payload, "h", &estado);
	protocol_packet_free(&paquete);

	*resultado_estado = estado;

	return true;
}

void atender_transformacion(unsigned char* payload) {
	estado_t estado;
	int numero_job;
	if(!procesar_transformacion(payload, &numero_job, &estado))
		pthread_exit(EXIT_FAILURE);

	if(estado == ESTADO_Error_Replanifica) {
		header_t cabecera;
		packet_t paquete;
		size_t size;

		//enviar Solicitar Transformacion
		char buffer[NUMERO_JOB_SIZE];
		size = serial_string_pack(buffer, "h", numero_job);
		cabecera = protocol_get_header(OP_YAM_Replanificar_Transformacion, size);
		paquete = protocol_get_packet(cabecera, &buffer);
		if(!protocol_packet_send(sock, &paquete))
			pthread_exit(EXIT_FAILURE);

		paquete = protocol_packet_receive(sock);
		if(paquete.header.operation == OP_ERROR)
			pthread_exit(EXIT_FAILURE);
		if(!procesar_transformacion(paquete.payload, &numero_job, &estado))
			pthread_exit(EXIT_FAILURE);
	}

	int r = estado == ESTADO_Finalizado_OK ? EXIT_SUCCESS : EXIT_FAILURE;
	pthread_exit(r);
}

void ejecutar_transformacion(socket_t sockYama, bool es_txt_transformador, char *arc_transformador, char *arc_origen) {
	sock = sockYama;
	es_txt_archivo_transformador = es_txt_transformador;
	nombre_archivo_transformador = arc_transformador;

	header_t cabecera;
	packet_t paquete;
	size_t size;

	//enviar Solicitar Transformacion
	char nombre_archivo[NOMBRE_ARCHIVO];
	size = serial_string_pack(&nombre_archivo, "s", arc_origen);
	cabecera = protocol_get_header(OP_YAM_Solicitar_Transformacion, size);
	paquete = protocol_get_packet(cabecera, &nombre_archivo);
	if(!protocol_packet_send(sock, &paquete))
		exit(EXIT_FAILURE);

	//recibir Solicitar Transformacion
	paquete = protocol_packet_receive(sock);
	if(paquete.header.operation == OP_ERROR)
		exit(EXIT_FAILURE);
	int i, cant_bloques;
	serial_string_unpack(paquete.payload, "h", &cant_bloques);
	protocol_packet_free(&paquete);

	pthread_t threads[cant_bloques];
	for(i = 0; i < cant_bloques; i++) {
		paquete = protocol_packet_receive(sock);
		if(paquete.header.operation == OP_ERROR)
			exit(EXIT_FAILURE);
		threads[i] = thread_create(atender_transformacion, paquete.payload);
	}
	int r;
	for(i = 0; i < cant_bloques; i++) {
		r = thread_join(threads[i]);
		if(r == EXIT_FAILURE)
			exit(EXIT_FAILURE);
	}
}

