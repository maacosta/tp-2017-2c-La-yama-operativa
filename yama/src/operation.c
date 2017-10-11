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

bool operation_iniciar_tarea(packet_t *packet, t_list *estados_master, socket_t cliente) {
	char archivo_a_procesar[250];
	serial_string_unpack(packet->payload, "s", &archivo_a_procesar);

	log_msg_info("Etapa Transformacion: archivo [ %s ]", archivo_a_procesar);

	//enviar consulta al FS sobre el archivo a procesar
	header_t c = protocol_get_header(OP_FSY_Informacion_Archivo, serial_pack(buffer, "s", &archivo_a_procesar));
	packet_t p = protocol_get_packet(c, &buffer);
	if(!protocol_packet_send(sockFS, &p)) {
		exit(EXIT_FAILURE);
	}
	//recibir informacion sobre el archivo a procesar del FS
	//el primer paquete contiene la cantidad de bloques
	p = protocol_packet_receive(sockFS);
	if(p.header.operation == OP_ERROR) {
		exit(EXIT_FAILURE);
	}
	int cant_bloques;
	serial_unpack(p.payload, "h", &cant_bloques);
	protocol_packet_free(&p);
	//los siguientes paquetes contiene el detalle de cada bloque
	int i;
	for(i = 0; i < cant_bloques; i++) {
		p = protocol_packet_receive(sockFS);
		if(p.header.operation == OP_ERROR) {
			exit(EXIT_FAILURE);
		}
		int num_bloque, tamanio;
		serial_unpack(p.payload, "hh", &num_bloque, &tamanio);
		estado_master_t *em = malloc(sizeof(estado_master_t));
		em->bloque = num_bloque;
		list_add(estados_master, em);
	}

	//consultar tablas
	return true;
}
