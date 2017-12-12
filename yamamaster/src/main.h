#ifndef MAIN_YAMAMASTER_H_
#define MAIN_YAMAMASTER_H_

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include "../../common/socket.h"
#include "../../common/protocol.h"
#include "configuration.h"

typedef struct {
	char ip[IP_SIZE];
	char puerto[PUERTO_SIZE];
	socket_t socket;
	sem_t semaphore;
} nodos_hilos_t;

#endif /* MAIN_YAMAMASTER_H_ */
