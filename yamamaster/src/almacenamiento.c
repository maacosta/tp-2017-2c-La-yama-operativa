#include "almacenamiento.h"

static socket_t sock;
static char *nombre_arch_destino;

void ejecutar_almacenamiento(socket_t sockYama, char *archivo_destino) {
	sock = sockYama;
	nombre_arch_destino = archivo_destino;

	header_t cabecera;
	packet_t paquete;
	size_t size;

	//enviar Solicitar Almacenamiento Final
	cabecera = protocol_get_header(OP_YAM_Solicitar_Almacenamiento_Final, 0);
	paquete = protocol_get_packet(cabecera, NULL);
	if(!protocol_packet_send(sock, &paquete))
		exit(EXIT_FAILURE);

	//recibir Solicitar Almacenamiento Final
	paquete = protocol_packet_receive(sock);
	if(paquete.header.operation == OP_ERROR)
		exit(EXIT_FAILURE);
	int num_job;
    char nombre_nodo[NOMBRE_NODO_SIZE];
    char ip[IP_SIZE];
    char puerto[PUERTO_SIZE];
    char nombre_archivo_tmp[NOMBRE_ARCHIVO_TMP];
	serial_string_unpack(paquete.payload, "h s s s s", &num_job, &nombre_nodo, &ip, &puerto, &nombre_archivo_tmp);
	protocol_packet_free(&paquete);

	socket_t sockWorker = conectar_con_worker(ip, puerto);

	//enviar Iniciar Almacenamiento Final
	char buffer[NOMBRE_ARCHIVO_TMP + NOMBRE_ARCHIVO_TMP + 1];
	size = serial_string_pack(&buffer, "s s", &nombre_archivo_tmp, archivo_destino);
	cabecera = protocol_get_header(OP_WRK_Iniciar_Almacenamiento_Final, size);
	paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(sockWorker, &paquete))
		exit(EXIT_FAILURE);

	//recibir Iniciar Almacenamiento Final
	paquete = protocol_packet_receive(sockWorker);
	if(paquete.header.operation == OP_ERROR)
		exit(EXIT_FAILURE);
	int respuesta;
	serial_string_unpack(paquete.payload, "h", &respuesta);
	protocol_packet_free(&paquete);

	//enviar Estado Almacenamiento Final
	char buffer2[NOMBRE_NODO_SIZE + RESPUESTA_SIZE + 1];
	size = serial_string_pack(buffer2, "h h", num_job, respuesta);
	cabecera = protocol_get_header(OP_YAM_Enviar_Estado, size);
	paquete = protocol_get_packet(cabecera, &buffer2);
	if(!protocol_packet_send(sock, &paquete))
		exit(EXIT_FAILURE);

	//recibir Estado Almacenamiento Final
	paquete = protocol_packet_receive(sock);
	if(paquete.header.operation == OP_ERROR)
		exit(EXIT_FAILURE);
	serial_string_unpack(paquete.payload, "h", &respuesta);
	protocol_packet_free(&paquete);

	if(respuesta != ESTADO_Finalizado_OK)
		exit(EXIT_FAILURE);
}

