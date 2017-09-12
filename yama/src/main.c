#include "main.h"
#include "../../common/log.h"


int main(int argc, char **argv) {
	log_init("log", "yama", true);

	socket_t srvfd = socket_listen("5000");
	log_msg_info("escuchando");

	header_t cabecera;
	if(protocol_handshake_receive(srvfd, &cabecera)) {
		log_msg_info("handshake ok");
	}
	else {
		log_msg_error("ocurrió un error en handshake");
	}

	return 0;
}
