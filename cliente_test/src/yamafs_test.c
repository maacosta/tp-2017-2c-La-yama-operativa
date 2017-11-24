#include "yamafs_test.h"

#define BLOQUE_LEN 1048576 //1024*1024

void probar_yamafs(socket_t sockO, socket_t sockD, int opcion) {
	char buffer[1024*4];
	packet_t packet;
	header_t cabecera;
	packet_t paquete;
	size_t size;

	packet = protocol_packet_receive(sockD);
	if(packet.header.operation == OP_ERROR) {
		socket_close(sockD);
		return;
	}

	switch(opcion) {
	case 100: {
		/*
		 * Recibir registracion de nodo, almacenar y obtener bloque de texto
		 */

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

		strcpy(buffer, "Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. ");
		cabecera = protocol_get_header(OP_DND_Almacenar_Bloque, string_length(&buffer) + 1);
		paquete = protocol_get_packet(cabecera, &buffer);
		if(!protocol_packet_send(sockD, &paquete))
			return;

		//packet.header.operation == OP_DND_Obtener_Bloque
		size = serial_string_pack(&buffer, "h", 3);
		cabecera = protocol_get_header(OP_DND_Obtener_Bloque, size);
		paquete = protocol_get_packet(cabecera, &buffer);
		if(!protocol_packet_send(sockD, &paquete))
			return;
		packet = protocol_packet_receive(sockD);
		if(packet.header.operation == OP_ERROR) {
			socket_close(sockD);
			return;
		}
		printf(packet.payload);
		protocol_packet_free(&packet);
		break;
	}
	case 200: {
		/*
		 * Recibir registracion de nodo, almacenar y obtener bloque binario
		 */

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

		strcpy(buffer, "Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. Esto es un texto de prueba desde el cliente_test. ");
		cabecera = protocol_get_header(OP_DND_Almacenar_Bloque, string_length(&buffer) + 1);
		paquete = protocol_get_packet(cabecera, &buffer);
		if(!protocol_packet_send(sockD, &paquete))
			return;

		//packet.header.operation == OP_DND_Obtener_Bloque
		size = serial_string_pack(&buffer, "h", 3);
		cabecera = protocol_get_header(OP_DND_Obtener_Bloque, size);
		paquete = protocol_get_packet(cabecera, &buffer);
		if(!protocol_packet_send(sockD, &paquete))
			return;
		packet = protocol_packet_receive(sockD);
		if(packet.header.operation == OP_ERROR) {
			socket_close(sockD);
			return;
		}
		printf(packet.payload);
		protocol_packet_free(&packet);
		break;
	}
	default:
		printf("Opcion no contemplada\n");
	}
}
