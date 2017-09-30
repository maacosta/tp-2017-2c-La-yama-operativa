/*
 * main.c
 *
 *  Created on: 4/9/2017
 *      Author: utnso
 */

#include "yamadatanodo.h"

yamaDataNodo_t *config;

int main(int argc, char **argv) {
	config = config_leer("metadata");
	conectarFileSystem();
	return 0;
}



void conectarFileSystem() {


	int socket_fs;


	if((socket_fs = socket_connect(config->ip_fs, config->puerto_fs)) == -1) {
			exit(EXIT_FAILURE);
	}
	if(!protocol_handshake_send(socket_fs, FS)) {
			exit(EXIT_FAILURE);
	}
	header_t header;
	if(!protocol_handshake_receive(socket_fs, &header)) {
		exit(EXIT_FAILURE);
	}


}
