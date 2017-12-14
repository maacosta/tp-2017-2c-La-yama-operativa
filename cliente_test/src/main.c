#include "main.h"

socket_t sockO, sockD;

socket_t conectar_con(char *ip, char *puerto, const char *nombre) {
	socket_t sock;
	if((sock = socket_connect(ip, puerto, nombre)) == -1) {
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

socket_t aceptar_cliente(socket_t server) {
	socket_t cliente = socket_accept(server);
	if(cliente == -1) return -1;

	header_t cabecera;
	if(!protocol_handshake_receive(cliente, &cabecera)) {
		socket_close(cliente);
		return -1;
	}
	if(!protocol_handshake_send(cliente)) {
		socket_close(cliente);
		return -1;
	}
	return cliente;
}

/*
 * cliente_test <proceso> <opcion> <ip> <puerto>
<proceso>
	YAMA = 1,
	FS = 2,
	MASTER = 3,
	DATANODE = 4,
	WORKER = 5
<ip> 127.0.0.1
 */
int main (int argc, char **argv)  {
	process_t p = atoi(argv[1]);
	int opcion = atoi(argv[2]);
	char *ip = argv[3];
	char *puerto = argv[4];

	global_set_process(p);
	log_init("log.txt", "test", true);

	switch((int)p) {
	case FS:
		switch(opcion) {
		case 0:
			yamafs_consola_leer_escribir_archivo_binario();
			break;
		case 1:
			yamafs_consola_leer_escribir_archivo_texto();
			break;
		case 2:
			sockO = socket_init(NULL, puerto, "YAMAFS");
			sockD = aceptar_cliente(sockO);

			yamafs_registrar_almacenar_obtener_txtbloque(sockO, sockD);
			break;
		case 3:
			sockO = socket_init(NULL, puerto, "YAMAFS");
			sockD = aceptar_cliente(sockO);

			yamafs_registrar_almacenar_obtener_binbloque(sockO, sockD);
			break;
		}
		break;
	case MASTER:
		switch(opcion) {
		case 0:
			sockD = conectar_con(ip, puerto, "WORKER");

			worker_enviar_transformacion(sockD);
			break;
		case 1:
			sockD = conectar_con(ip, puerto, "WORKER");

			worker_enviar_reduccion(sockD);
			break;
		}
		break;
	}
	return EXIT_SUCCESS;
}
