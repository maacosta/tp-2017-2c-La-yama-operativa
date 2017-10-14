#include "reduccion.h"

static socket_t sock;
static char *nombre_arch_reductor;
static char *nombre_arch_origen;

void atender_reduccion(unsigned char* payload) {
    char nombre_nodo[NOMBRE_NODO_SIZE];
    char ip[IP_SIZE];
    char puerto[PUERTO_SIZE];
    char nombre_archivos_tmp[NOMBRE_ARCHIVO_TMP*10];
    char nombre_archivo_reduccion_local[NOMBRE_ARCHIVO_TMP];

	serial_string_unpack(payload, "s s s s s", &nombre_nodo, &ip, &puerto, &nombre_archivos_tmp, &nombre_archivo_reduccion_local);
	free(payload);

	socket_t sockWorker = conectar_con_worker(ip, puerto);

	header_t cabecera;
	packet_t paquete;
	size_t size;

	//enviar programa Reductor
	char *arc_reduc;
	size = global_readfile(nombre_arch_reductor, arc_reduc);
	cabecera = protocol_get_header(OP_WRK_Iniciar_Reduccion, size);
	paquete = protocol_get_packet(cabecera, arc_reduc);
	if(!protocol_packet_send(sockWorker, &paquete))
		exit(EXIT_FAILURE);
	free(arc_reduc);

	//enviar Iniciar Reduccion
	char buffer[NOMBRE_ARCHIVO_TMP + NOMBRE_ARCHIVO_TMP*10 + 1];
	size = serial_string_pack(&buffer, "s s", &nombre_archivos_tmp, &nombre_archivo_reduccion_local);
	cabecera = protocol_get_header(OP_WRK_Iniciar_Reduccion, size);
	paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(sockWorker, &paquete))
		exit(EXIT_FAILURE);

	//recibir Iniciar Reduccion
	paquete = protocol_packet_receive(sockWorker);
	if(paquete.header.operation == OP_ERROR)
		exit(EXIT_FAILURE);
	int respuesta;
	serial_string_unpack(paquete.payload, "h", &respuesta);
	protocol_packet_free(&paquete);

	//enviar Estado Transformacion
	char buffer2[NOMBRE_NODO_SIZE + RESPUESTA_SIZE + 1];
	size = serial_string_pack(buffer2, "s h", nombre_nodo, respuesta);
	cabecera = protocol_get_header(OP_YAM_Enviar_Estado_Reduccion, size);
	paquete = protocol_get_packet(cabecera, &buffer2);
	if(!protocol_packet_send(sock, &paquete))
		exit(EXIT_FAILURE);
}

void ejecutar_reduccion(socket_t sockYama, char *archivo_reductor, char *archivo_origen) {
	sock = sockYama;
	nombre_arch_reductor= archivo_reductor;
	char nombre_archivo[NOMBRE_ARCHIVO];
	nombre_arch_origen = archivo_origen;

	header_t cabecera;
	packet_t paquete;
	size_t size;

	//enviar Solicitar Reduccion
	size = serial_string_pack(nombre_archivo, "s", archivo_origen);
	cabecera = protocol_get_header(OP_YAM_Solicitar_Reduccion, size);
	paquete = protocol_get_packet(cabecera, &nombre_archivo);
	if(!protocol_packet_send(sock, &paquete))
		exit(EXIT_FAILURE);

	//recibir Solicitar Reduccion
	paquete = protocol_packet_receive(sock);
	if(paquete.header.operation == OP_ERROR)
		exit(EXIT_FAILURE);
	int i, cant_reducciones;
	serial_string_unpack(paquete.payload, "h", &cant_reducciones);
	protocol_packet_free(&paquete);

	pthread_t threads[cant_reducciones];
	for(i = 0; i < cant_reducciones; i++) {
		paquete = protocol_packet_receive(sock);
		if(paquete.header.operation == OP_ERROR)
			exit(EXIT_FAILURE);
		threads[i] = thread_create(atender_reduccion, paquete.payload);
	}
	for(i = 0; i < cant_reducciones; i++)
		thread_join(threads[i]);
}

void ejecutar_reduccion_global(socket_t sockYama, char *archivo_reductor, char *archivo_origen) {
	sock = sockYama;
	nombre_arch_reductor= archivo_reductor;
	char nombre_archivo[NOMBRE_ARCHIVO];
	nombre_arch_origen = archivo_origen;

	header_t cabecera;
	packet_t paquete;
	size_t size;

	//enviar Solicitar Reduccion Global
	size = serial_string_pack(nombre_archivo, "s", archivo_origen);
	cabecera = protocol_get_header(OP_YAM_Solicitar_Reduccion_Global, size);
	paquete = protocol_get_packet(cabecera, &nombre_archivo);
	if(!protocol_packet_send(sock, &paquete))
		exit(EXIT_FAILURE);

	//recibir Solicitar Reduccion Global
	paquete = protocol_packet_receive(sock);
	if(paquete.header.operation == OP_ERROR)
		exit(EXIT_FAILURE);
	int i, cant_reducciones;
	serial_string_unpack(paquete.payload, "h", &cant_reducciones);
	protocol_packet_free(&paquete);

	reduccion_global_t reducciones[cant_reducciones];
	int j;
	for(i = 0; i < cant_reducciones; i++) {
		paquete = protocol_packet_receive(sock);
		if(paquete.header.operation == OP_ERROR)
			exit(EXIT_FAILURE);
		serial_string_unpack(paquete.payload, "s s s s s h", &reducciones[i].nombre_nodo, &reducciones[i].ip, &reducciones[i].puerto, &reducciones[i].nombre_archivo_local, &reducciones[i].nombre_archivo_global, &reducciones[i].encargado);
		protocol_packet_free(&paquete);
		if(reducciones[i].encargado)
			j = i;
	}

	socket_t sockWorker = conectar_con_worker(reducciones[j].ip, reducciones[j].puerto);

	//enviar programa Reductor
	char *arc_reduc;
	size = global_readfile(nombre_arch_reductor, arc_reduc);
	cabecera = protocol_get_header(OP_WRK_Iniciar_Reduccion_Global, size);
	paquete = protocol_get_packet(cabecera, arc_reduc);
	if(!protocol_packet_send(sockWorker, &paquete))
		exit(EXIT_FAILURE);
	free(arc_reduc);

	//enviar Iniciar Reduccion Global
	char buffer[NOMBRE_ARCHIVO_TMP + BLOQUE_SIZE_E + 1];
	size = serial_string_pack(&buffer, "s h", &reducciones[j].nombre_archivo_global, cant_reducciones);
	cabecera = protocol_get_header(OP_WRK_Iniciar_Reduccion_Global, size);
	paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(sockWorker, &paquete))
		exit(EXIT_FAILURE);
	char buffer2[IP_SIZE + PUERTO_SIZE + NOMBRE_ARCHIVO_TMP + 2];
	for(i = 0; i < cant_reducciones; i++) {
		size = serial_string_pack(&buffer2, "s s s", &reducciones[i].ip, &reducciones[i].puerto, &reducciones[i].nombre_archivo_local);
		cabecera = protocol_get_header(OP_WRK_Iniciar_Reduccion_Global, size);
		paquete = protocol_get_packet(cabecera, &buffer2);
		if(!protocol_packet_send(sockWorker, &paquete))
			exit(EXIT_FAILURE);
	}
	//recibir Iniciar Reduccion Global
	paquete = protocol_packet_receive(sockWorker);
	if(paquete.header.operation == OP_ERROR)
		exit(EXIT_FAILURE);
	int respuesta;
	serial_string_unpack(paquete.payload, "h", &respuesta);
	protocol_packet_free(&paquete);

	//enviar Estado Transformacion
	char buffer3[NOMBRE_NODO_SIZE + RESPUESTA_SIZE + 1];
	size = serial_string_pack(buffer3, "s h", &reducciones[j].nombre_nodo, respuesta);
	cabecera = protocol_get_header(OP_YAM_Enviar_Estado_Reduccion_Global, size);
	paquete = protocol_get_packet(cabecera, &buffer3);
	if(!protocol_packet_send(sock, &paquete))
		exit(EXIT_FAILURE);
}
