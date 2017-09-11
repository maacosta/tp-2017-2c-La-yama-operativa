#include "main.h"

void conectar_con_yama() {
	socket_t sockfd = socket_connect("ip_yama", "puerto");

	header_t header;
	header.process = MASTER;

	protocol_handshake_send(sockfd, header);

	//enviar operacion de tarea

}

int main(int argc, char **argv) {
	conectar_con_yama();


	return EXIT_SUCCESS;
}
