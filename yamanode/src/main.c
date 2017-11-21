#include "main.h"

yamaDataNodo_t *config;
socket_t sockFS;

socket_t conectar_con_yamafs(yamaDataNodo_t *config) {
	socket_t sock;
	if((sock = socket_connect(config->yamafs_ip, config->yamafs_puerto)) == -1) {
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

void atender_solicitudes() {
	header_t cabecera;
	packet_t paquete;
	size_t size;

	//registrar
	char buffer[NOMBRE_NODO_SIZE + BLOQUE_SIZE_E + 1];
	size = serial_string_pack(&buffer, "s h", config->nombreNodo, 98);
	cabecera = protocol_get_header(OP_FSY_Registrar_Nodo, size);
	paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(sockFS, &paquete))
		exit(EXIT_FAILURE);

	//atender solicitudes de yamafs
	while(true) {
		packet_t packet = protocol_packet_receive(sockFS);
		if(packet.header.operation == OP_ERROR) {
			socket_close(sockFS);
			exit(EXIT_FAILURE);
		}
		switch(packet.header.operation) {
		case OP_DND_Obtener_Bloque:
			break;
		case OP_DND_Almacenar_Bloque:
			break;
		}
	}
}

int main(int argc, char **argv) {
	global_set_process(DATANODE);
	config = config_leer("metadata");
	log_init(config->log_file, config->log_name, true);

	memoria_abrir(config);

	printf("mmap size of [ %d ] bytes, [ %d ] kbytes, [ %d ] mbytes\n", memoria_obtener_tamanio(), memoria_obtener_tamanio() / 1024, memoria_obtener_tamanio() / (1024 * 1024));

	char *txt = memoria_obtener_bloque(2, 1024);
	memcpy(txt, "todo un loco", 1024);
	memoria_almacenar_bloque(2, 1024, txt);

	memoria_almacenar_bloque(1, 1024, "alejandro genio");

	char *txt2 = memoria_obtener_bloque(1, 1024);

	memoria_destruir();
	/*
	sockFS = conectar_con_yamafs(config);

	if(sockFS != -1)
		atender_solicitudes();
*/
	config_liberar(config);

	return EXIT_SUCCESS;
}
