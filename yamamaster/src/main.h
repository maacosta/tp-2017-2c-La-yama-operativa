#ifndef MAIN_YAMAMASTER_H_
#define MAIN_YAMAMASTER_H_

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include "../../common/socket.h"
#include "../../common/protocol.h"
#include "configuration.h"
#include "op_transformacion.h"
#include "op_reduccion.h"
#include "op_almacenamiento.h"

typedef struct {
	char ip[IP_SIZE];
	char puerto[PUERTO_SIZE];
	socket_t socket;
	sem_t semaphore;
} nodos_hilos_t;

socket_t conectar_con_worker(char *ip, char *puerto);

#endif /* MAIN_YAMAMASTER_H_ */
