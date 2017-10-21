#include "main.h"

int sockworker;

socket_t conectar_con_worker_prueba(char *ip, char *puerto) {
	socket_t sock;
	if((sock = socket_connect(ip, puerto)) == -1) {
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

int main (int argc, char **argv)  {

	sockworker = conectar_con_worker_prueba("127.0.0.1","5005");


}
