#include "op_almacenamiento.h"

void ejecutar_almacenamiento(socket_t sockYama, char *archivo_destino, int num_job) {
	header_t cabecera;
	packet_t paquete;
	size_t size;

	log_msg_info("Almacenamiento Final: Job [ %d ] [ %s ]", num_job, archivo_destino);

	//enviar Solicitar Almacenamiento Final
	char buffer[NUMERO_JOB_SIZE];
	size = serial_string_pack(&buffer, "h", num_job);
	cabecera = protocol_get_header(OP_YAM_Solicitar_Almacenamiento_Final, size);
	paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(sockYama, &paquete))
		exit(EXIT_FAILURE);

	//recibir Solicitar Almacenamiento Final
	paquete = protocol_packet_receive(sockYama);
	if(paquete.header.operation == OP_ERROR)
		exit(EXIT_FAILURE);
    char nombre_nodo[NOMBRE_NODO_SIZE];
    char ip[IP_SIZE];
    char puerto[PUERTO_SIZE];
    char nombre_archivo_tmp[NOMBRE_ARCHIVO_TMP];
	serial_string_unpack(paquete.payload, "s s s s", &nombre_nodo, &ip, &puerto, &nombre_archivo_tmp);
	protocol_packet_free(&paquete);

	socket_t sockWorker = conectar_con_worker(ip, puerto);
	if(sockWorker == -1)
		exit(EXIT_FAILURE);

	//enviar Iniciar Almacenamiento Final
	char buffer2[NOMBRE_ARCHIVO_TMP + NOMBRE_ARCHIVO_TMP + 1];
	size = serial_string_pack(&buffer2, "s s", &nombre_archivo_tmp, archivo_destino);
	cabecera = protocol_get_header(OP_WRK_Iniciar_Almacenamiento_Final, size);
	paquete = protocol_get_packet(cabecera, &buffer2);
	if(!protocol_packet_send(sockWorker, &paquete)) {
		socket_close(sockWorker);
		exit(EXIT_FAILURE);
	}

	//recibir Iniciar Almacenamiento Final
	paquete = protocol_packet_receive(sockWorker);
	if(paquete.header.operation == OP_ERROR)
		exit(EXIT_FAILURE);
	resultado_t respuesta;
	serial_string_unpack(paquete.payload, "h", &respuesta);
	protocol_packet_free(&paquete);

	//enviar Estado Almacenamiento Final
	char buffer3[NUMERO_JOB_SIZE + NOMBRE_NODO_SIZE + BLOQUE_SIZE_E + RESPUESTA_SIZE + 3];
	size = serial_string_pack(&buffer3, "h s h h", num_job, &nombre_nodo, 0, respuesta);
	cabecera = protocol_get_header(OP_YAM_Enviar_Estado, size);
	paquete = protocol_get_packet(cabecera, &buffer3);
	if(!protocol_packet_send(sockYama, &paquete))
		exit(EXIT_FAILURE);

	//recibir Estado Almacenamiento Final
	paquete = protocol_packet_receive(sockYama);
	if(paquete.header.operation == OP_ERROR)
		exit(EXIT_FAILURE);
	estado_t estado;
	serial_string_unpack(paquete.payload, "h", &estado);
	protocol_packet_free(&paquete);

	log_msg_info("Almacenamiento Final: Finalizacion [ %s ]: Job [ %d ]", estado == ESTADO_Finalizado_OK ? "OK" : "ERROR", num_job);

	if(estado != ESTADO_Finalizado_OK)
		exit(EXIT_FAILURE);
}

