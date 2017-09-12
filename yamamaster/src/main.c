#include "main.h"

void conectar_con_yama() {
	socket_t sockfd = socket_connect("127.0.0.1", "5000");

	header_t header;
	header.process = MASTER;

	protocol_handshake_send(sockfd, MASTER);

	//enviar operacion de tarea

}

int main(int argc, char **argv) {
	conectar_con_yama();


	return EXIT_SUCCESS;
}
