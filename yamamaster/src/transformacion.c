#include "transformacion.h"

unsigned char buffer[4096];

void ejecutar_transformacion(socket_t sock, char *archivo_transformador, char *archivo_origen) {
	header_t cabecera = protocol_get_header(OP_MASTER_TRANSFORMACION, serial_string_pack(buffer, "s", archivo_origen));
	packet_t paquete = protocol_get_packet(cabecera, &buffer);
	protocol_packet_send(sock, &paquete);
}
