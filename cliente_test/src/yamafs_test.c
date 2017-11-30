#include "yamafs_test.h"

packet_t packet;
header_t cabecera;
packet_t paquete;
size_t size;
char buffer[1024*4];

/*
 * Recibir registracion de nodo, almacenar y obtener bloque de texto
 */
void yamafs_registrar_almacenar_obtener_txtbloque(socket_t sockO, socket_t sockD) {
	int lentxt;
	packet = protocol_packet_receive(sockD);
	if(packet.header.operation == OP_ERROR) {
		socket_close(sockD);
		return;
	}

	//packet.header.process == DATANODE
	//packet.header.operation == OP_FSY_Registrar_Nodo
	char nombre_nodo[NOMBRE_NODO_SIZE];
	int cant_bloques;
	serial_string_unpack(packet.payload, "s h", &nombre_nodo, &cant_bloques);
	protocol_packet_free(&packet);
	printf("nombre_nodo: %s, cant_bloques: %d\n", nombre_nodo, cant_bloques);

	//packet.header.operation == OP_DND_Almacenar_Bloque
	size = serial_string_pack(&buffer, "h", 3);
	cabecera = protocol_get_header(OP_DND_Almacenar_Bloque, size);
	paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(sockD, &paquete))
		return;

	strcpy(buffer, "Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test.");
	lentxt = string_length(&buffer);
	cabecera = protocol_get_header(OP_DND_Almacenar_Bloque, lentxt);
	paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(sockD, &paquete))
		return;

	//packet.header.operation == OP_DND_Obtener_Bloque
	size = serial_string_pack(&buffer, "h h", 3, lentxt);
	cabecera = protocol_get_header(OP_DND_Obtener_Bloque, size);
	paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(sockD, &paquete))
		return;
	packet = protocol_packet_receive(sockD);
	if(packet.header.operation == OP_ERROR) {
		socket_close(sockD);
		return;
	}
	printf(string_equals_ignore_case(buffer, packet.payload) ? "Mismo texto" : "TEXTOS DIFERENTES");
	printf(packet.payload);
	protocol_packet_free(&packet);
}

/*
 * Recibir registracion de nodo, almacenar y obtener bloque binario
 */
void yamafs_registrar_almacenar_obtener_binbloque(socket_t sockO, socket_t sockD) {
	char *path = "./penguin_linux.jpeg";
	char *path_copy = "./copy_penguin_linux.jpeg";

	ssize_t sizebin;
	unsigned char *bufferbin = global_read_binfile(path, &sizebin);

	packet = protocol_packet_receive(sockD);
	if(packet.header.operation == OP_ERROR) {
		socket_close(sockD);
		return;
	}

	//packet.header.process == DATANODE
	//packet.header.operation == OP_FSY_Registrar_Nodo
	char nombre_nodo[NOMBRE_NODO_SIZE];
	int cant_bloques;
	serial_string_unpack(packet.payload, "s h", &nombre_nodo, &cant_bloques);
	protocol_packet_free(&packet);
	printf("nombre_nodo: %s, cant_bloques: %d\n", nombre_nodo, cant_bloques);

	//packet.header.operation == OP_DND_Almacenar_Bloque
	size = serial_string_pack(&buffer, "h", 2);
	cabecera = protocol_get_header(OP_DND_Almacenar_Bloque, size);
	paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(sockD, &paquete))
		return;

	cabecera = protocol_get_header(OP_DND_Almacenar_Bloque, sizebin);
	paquete = protocol_get_packet(cabecera, bufferbin);
	if(!protocol_packet_send(sockD, &paquete))
		return;

	//packet.header.operation == OP_DND_Obtener_Bloque
	size = serial_string_pack(&buffer, "h h", 2, sizebin);
	cabecera = protocol_get_header(OP_DND_Obtener_Bloque, size);
	paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(sockD, &paquete))
		return;
	packet = protocol_packet_receive(sockD);
	if(packet.header.operation == OP_ERROR) {
		socket_close(sockD);
		return;
	}

	if(global_get_file_exist(path_copy))
		global_delete_file(path_copy);
	global_create_binfile(path_copy, packet.payload, packet.header.size);

	protocol_packet_free(&packet);
}
