#include "almacenamiento.h"

static socket_t sock;
static char *nombre_arch_destino;
static char *nombre_arch_origen;

void ejecutar_almacenamiento(socket_t sockYama, char *archivo_destino, char *archivo_origen) {
	sock = sockYama;
	nombre_arch_destino = archivo_destino;
	char nombre_archivo[NOMBRE_ARCHIVO];
	nombre_arch_origen = archivo_origen;

	header_t cabecera;
	packet_t paquete;
	size_t size;

	//enviar Solicitar Almacenamiento Final
	size = serial_string_pack(nombre_archivo, "s", archivo_origen);
	cabecera = protocol_get_header(OP_YAM_Solicitar_Almacenamiento_Final, size);
	paquete = protocol_get_packet(cabecera, &nombre_archivo);
	if(!protocol_packet_send(sock, &paquete))
		exit(EXIT_FAILURE);

	//recibir Solicitar Almacenamiento Final
	paquete = protocol_packet_receive(sock);
	if(paquete.header.operation == OP_ERROR)
		exit(EXIT_FAILURE);
    char nombre_nodo[NOMBRE_NODO_SIZE];
    char ip[IP_SIZE];
    char puerto[PUERTO_SIZE];
    char nombre_archivo_tmp[NOMBRE_ARCHIVO_TMP];
	serial_string_unpack(paquete.payload, "s s s s", &nombre_nodo, &ip, &puerto, &nombre_archivo_tmp);
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
	size = serial_string_pack(buffer2, "s h", &nombre_nodo, respuesta);
	cabecera = protocol_get_header(OP_YAM_Enviar_Estado_Almacenamiento_Final, size);
	paquete = protocol_get_packet(cabecera, &buffer2);
	if(!protocol_packet_send(sock, &paquete))
		exit(EXIT_FAILURE);
}

