#include "transformacion.h"

static unsigned char buffer[4096];
static socket_t sock;
static char *ar_transformador;
static char *ar_origen;

socket_t conectar_con_worker(char *ip, char *puerto) {
	socket_t sock;
	if((sock = socket_connect(ip, puerto)) == -1) {
		exit(EXIT_FAILURE);
	}

	if(!protocol_handshake_send(sock)) {
		exit(EXIT_FAILURE);
	}
	header_t header;
	if(!protocol_handshake_receive(sock, &header)) {
		exit(EXIT_FAILURE);
	}
	return sock;
}

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

	//enviar Iniciar Transformacion
	size = serial_string_pack(buffer, "h h s", bloque, bytes_ocupados, &nombre_archivo_tmp);
	cabecera = protocol_get_header(OP_WRK_Iniciar_Transformacion, size);
	paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(sock, &paquete))
		exit(EXIT_FAILURE);
}

void ejecutar_transformacion(socket_t sockYama, char *archivo_transformador, char *archivo_origen) {
	sock = sockYama;
	ar_transformador = archivo_transformador;
	ar_origen = archivo_origen;

	header_t cabecera;
	packet_t paquete;
	size_t size;

	//enviar Solicitar Transformacion
	size = serial_string_pack(buffer, "s", archivo_origen);
	cabecera = protocol_get_header(OP_YAM_Solicitar_Transformacion, size);
	paquete = protocol_get_packet(cabecera, &buffer);
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
	for(i = 0; i < cant_bloques; i++) {
		thread_join(threads[i]);
	}
}

