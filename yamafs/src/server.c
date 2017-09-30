#include "server.h"


void server_crear(yamafs_t *config) {
	pthread_t th_server;

	struct arg_struct args;
	args.puerto_listen = config->puerto;


	pthread_create(&th_server, NULL, (void*)server_iniciar, (void*) config->puerto);


}

//void server_iniciar(void* argumentos) {
void server_iniciar(void* puerto) {


	char* puerto_listen = puerto;
	//printf("el prueto es %d\n",atoi(puerto_listen));

	fd_set master;    // master file descriptor list
	fd_set read_fds;  // temp file descriptor list for select()
	int fdmax;        // maximum file descriptor number
	int newfd;        	   // newly accept()ed socket descriptor
	int socket_actual;
	int socket_yama = 0;

	struct sockaddr_storage addr_client; // client address
	socklen_t addrlen;

	FD_ZERO(&master);    // clear the master and temp sets
	FD_ZERO(&read_fds);


	int listen_socket = socket_listen(puerto_listen);

	// add the listener to the master set
	FD_SET(listen_socket, &master);

	// keep track of the biggest file descriptor
	fdmax = listen_socket; // so far, it's this one

	while (1) {

		read_fds = master; // copy it
		if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
			//log_error_consola("Fallo el select");
			perror("Fallo el select. Error");
		}

		//correr por las conexiones existentes en busca de datos para leer
		for (socket_actual = 0; socket_actual <= fdmax; socket_actual++) {
			if (FD_ISSET(socket_actual, &read_fds)) {
				if (socket_actual == listen_socket) {//si es el socket de escucha

					// handle new connections

					addrlen = sizeof addr_client;
					newfd = accept(listen_socket, (struct sockaddr *) &addr_client, &addrlen);
					if (newfd == -1) {
						log_msg_error("fallo el acept");

					}
					else{
						FD_SET(newfd, &master);//agrego el nuevo socket a la bolsa master
						if (newfd > fdmax) {
									fdmax = newfd;
						}

						header_t cabecera;
						if(!protocol_handshake_receive(newfd, &cabecera)) {
											socket_close(newfd);
											continue;
						}
						if(cabecera.process == YAMA) {
							if (fsOperativo) {

								if(!protocol_handshake_send(newfd)) {
									log_msg_error("fallo el handshake send con YAMA");
									socket_close(newfd);
									//aca tambien tendría que limpiar a newfd de la master? FD_CLR(newfd, &master);
								}
								socket_yama = newfd;
								yamaEstaConectada = true;
								log_msg_info("Yama se conecto correctamente");
							}else{
								//mandar el msj a yama que tiene que esperar porque el fs aun no esta operativo
								log_msg_error("Yama intento conectarse pero fs aun no esta operativo");
								socket_close(newfd);
								FD_CLR(newfd, &master);
							}
						}//Fin handshake con YAMA

						if(cabecera.process == DATANODE){
							if(!protocol_handshake_send(newfd)) {
								log_msg_error("fallo el handshake send con DATANODE");
								socket_close(newfd);
								//aca tambien tendría que limpiar a newfd de la master? FD_CLR(newfd, &master);
							}
						}
					}

				}//fin si es el socket de escucha
				else{// Este else se utiliza cuando no es una nueva conexion, es decir que se quiere hacer algo



				}
			}
		}

	}


}
