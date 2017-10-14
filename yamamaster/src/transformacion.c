#include "transformacion.h"

static socket_t sock;
static char *nombre_arch_transformador;
static char *nombre_arch_origen;

void atender_transformacion(unsigned char* payload) {
    char nombre_nodo[NOMBRE_NODO_SIZE];
    char ip[IP_SIZE];
    char puerto[PUERTO_SIZE];
    int bloque;
    int bytes_ocupados;
    char nombre_archivo_tmp[NOMBRE_ARCHIVO_TMP];

	serial_string_unpack(payload, "s s s h h s", &nombre_nodo, &ip, &puerto, &bloque, &bytes_ocupados, &nombre_archivo_tmp);
	free(payload);

	socket_t sockWorker = conectar_con_worker(ip, puerto);

	header_t cabecera;
	packet_t paquete;
	size_t size;

	//enviar programa Transformacion
	char *arc_trans;
	size = global_readfile(nombre_arch_transformador, arc_trans);
	cabecera = protocol_get_header(OP_WRK_Iniciar_Transformacion, size);
	paquete = protocol_get_packet(cabecera, arc_trans);
	if(!protocol_packet_send(sockWorker, &paquete))
		exit(EXIT_FAILURE);
	free(arc_trans);

	//enviar Iniciar Transformacion
	char buffer[BLOQUE_SIZE_E + BYTES_OCUPADOS_SIZE_E + NOMBRE_ARCHIVO_TMP + 2];
	size = serial_string_pack(&buffer, "h h s", bloque, bytes_ocupados, &nombre_archivo_tmp);
	cabecera = protocol_get_header(OP_WRK_Iniciar_Transformacion, size);
	paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(sockWorker, &paquete))
		exit(EXIT_FAILURE);

	//recibir Iniciar Transformacion
	paquete = protocol_packet_receive(sockWorker);
	if(paquete.header.operation == OP_ERROR)
		exit(EXIT_FAILURE);
	int respuesta;
	serial_string_unpack(paquete.payload, "h", &respuesta);
	protocol_packet_free(&paquete);

	//enviar Estado Transformacion
	char buffer2[NOMBRE_NODO_SIZE + BLOQUE_SIZE_E + RESPUESTA_SIZE + 2];
	size = serial_string_pack(buffer2, "s h h", &nombre_nodo, bloque, respuesta);
	cabecera = protocol_get_header(OP_YAM_Enviar_Estado_Transformacion, size);
	paquete = protocol_get_packet(cabecera, &buffer2);
	if(!protocol_packet_send(sock, &paquete))
		exit(EXIT_FAILURE);
}

void ejecutar_transformacion(socket_t sockYama, char *archivo_transformador, char *archivo_origen) {
	sock = sockYama;
	nombre_arch_transformador = archivo_transformador;
	char nombre_archivo[NOMBRE_ARCHIVO];
	nombre_arch_origen = archivo_origen;

	header_t cabecera;
	packet_t paquete;
	size_t size;

	//enviar Solicitar Transformacion
	size = serial_string_pack(nombre_archivo, "s", archivo_origen);
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
	for(i = 0; i < cant_bloques; i++)
		thread_join(threads[i]);
}

