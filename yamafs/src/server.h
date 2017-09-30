/*
 * server.h
 *
 *  Created on: 19/9/2017
 *      Author: utnso
 */

#ifndef SRC_SERVER_H_
#define SRC_SERVER_H_

struct arg_struct {
	char* puerto_listen;
};

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <commons/config.h>
#include "../../common/protocol.h"
#include "configuration.h"
#include <pthread.h>


bool fsOperativo;

bool yamaEstaConectada;

void server_crear(yamafs_t *config);
void server_iniciar(void* argumentos);
#endif /* SRC_SERVER_H_ */
