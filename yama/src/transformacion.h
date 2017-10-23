#ifndef YAMA_SRC_TRANSFORMACION_H_
#define YAMA_SRC_TRANSFORMACION_H_

#include <stdbool.h>
#include <commons/collections/list.h>
#include "../../common/global.h"
#include "../../common/socket.h"
#include "../../common/protocol.h"
#include "configuration.h"
#include "operation.h"

typedef struct {
	char nodo[NOMBRE_NODO_SIZE];
	unsigned int availability;
} planificacion_t;

void transformacion_iniciar(packet_t *packet, socket_t sockMaster, socket_t sockFS, yama_t* config, t_list *estados_master, t_list *nodos);

#endif /* YAMA_SRC_TRANSFORMACION_H_ */
