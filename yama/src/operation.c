#include "operation.h"

static unsigned char buffer[4096];
static yama_t* config;
static socket_t sockYAMA;
static socket_t sockFS;

void operation_init(yama_t* c, socket_t sockyama, socket_t sockfs) {
	config = c;
	sockYAMA = sockyama;
	sockFS = sockfs;
}

bool operation_solicitar_transformacion(packet_t *packet, t_list *estados_master, socket_t cliente) {
	char archivo_a_procesar[250];
	serial_string_unpack(packet->payload, "s", &archivo_a_procesar);

	log_msg_info("Etapa Transformacion: archivo [ %s ]", archivo_a_procesar);

	header_t cabecera;
	packet_t paquete;
	size_t size;

	//TODO: HARDCODE
    char nombre_nodo[NOMBRE_NODO_SIZE];
    char ip[IP_SIZE];
    char puerto[PUERTO_SIZE];
    int bloque;
    int bytes_ocupados;
    char nombre_archivo_tmp[NOMBRE_ARCHIVO_TMP];

	//enviar Solicitar Transformacion
	size = serial_string_pack(buffer, "h", 2);
	cabecera = protocol_get_header(OP_YAM_Solicitar_Transformacion, size);
	paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(cliente, &paquete))
		exit(EXIT_FAILURE);

	size = serial_string_pack(buffer, "s s s h h s", "nodo1", "127.0.0.1", "5000", 38, 10180, "/tmp/Master1-temp38");
	cabecera = protocol_get_header(OP_YAM_Solicitar_Transformacion, size);
	paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(cliente, &paquete))
		exit(EXIT_FAILURE);

	size = serial_string_pack(buffer, "s s s h h s", "nodo 2", "127.0.0.1", "5000", 44, 1048576, "/tmp/Master1-temp44");
	cabecera = protocol_get_header(OP_YAM_Solicitar_Transformacion, size);
	paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(cliente, &paquete))
		exit(EXIT_FAILURE);

	//enviar consulta Informacion Archivo
	/*header_t c = protocol_get_header(OP_FSY_Informacion_Archivo, serial_pack(buffer, "s", &archivo_a_procesar));
	packet_t p = protocol_get_packet(c, &buffer);
	if(!protocol_packet_send(sockFS, &p))
		exit(EXIT_FAILURE);*/

	//recibir Informacion Archivo
	/*p = protocol_packet_receive(sockFS);
	if(p.header.operation == OP_ERROR)
		exit(EXIT_FAILURE);
	int cant_bloques;
	serial_unpack(p.payload, "h", &cant_bloques);
	protocol_packet_free(&p);*/
	//detalle de cada bloque
	/*int i;
	for(i = 0; i < cant_bloques; i++) {
		p = protocol_packet_receive(sockFS);
		if(p.header.operation == OP_ERROR)
			exit(EXIT_FAILURE);
		int num_bloque, tamanio;
		serial_unpack(p.payload, "hh", &num_bloque, &tamanio);
		estado_master_t *em = malloc(sizeof(estado_master_t));
		em->bloque = num_bloque;
		list_add(estados_master, em);
	}*/


	//consultar tablas
	return true;
}
