/*
 * server.h
 *
 *  Created on: 3/10/2017
 *      Author: utnso
 */

#ifndef SRC_SERVER_H_
#define SRC_SERVER_H_

#include <stdbool.h>
#include <commons/collections/list.h>
#include "../../common/global.h"
#include "../../common/socket.h"
#include "../../common/protocol.h"
#include "configuration.h"
#include "operations.h"

typedef struct{
	int job;
	int master;
	char nodo[50];
	int bloque;
	char etapa;					//transformacion, reduccion local, reduccion global, almacenamiento final
	char archivo_temporal[50];	//el path es temp/nombre_aleatorio hasta 255
	char estado;				//en proceso, finalizado ok, error
} estado_master_t;

socket_t escuchar_master();

void server_liberar();



#endif /* SRC_SERVER_H_ */
