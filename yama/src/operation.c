#include "operation.h"

static unsigned char buffer[1024];
static yama_t* config;
static socket_t sockYAMA;
static socket_t sockFS;

void operation_init(yama_t* c, socket_t sockyama, socket_t sockfs) {
	config = c;
	sockYAMA = sockyama;
	sockFS = sockfs;
}

bool operation_iniciar_tarea(packet_t *packet) {
	char archivo_a_procesar[250];
	serial_unpack(packet->payload, "s", &archivo_a_procesar);

	//enviar consulta al FS sobre el archivo a procesar
	header_t c = protocol_get_header(OP_YAMA_INFO_ARCHIVO, serial_pack(buffer, "s", &archivo_a_procesar));
	packet_t p = protocol_get_packet(c, &buffer);
	protocol_packet_send(sockFS, &p);
	//recibir informacion sobre el archivo a procesar del FS
	p = protocol_packet_receive(sockFS);
	if(p.header.operation == OP_ERROR) {
		socket_close(sockFS);
		return false;
	}

	//consultar tablas
	return true;
}
