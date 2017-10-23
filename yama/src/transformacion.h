#ifndef YAMA_SRC_TRANSFORMACION_H_
#define YAMA_SRC_TRANSFORMACION_H_

#include <stdbool.h>
#include <commons/collections/list.h>
#include "../../common/global.h"
#include "../../common/socket.h"
#include "../../common/protocol.h"
#include "configuration.h"
#include "filesystem.h"

typedef struct {
	char nodo[NOMBRE_NODO_SIZE];
	unsigned int availability;
} planificacion_t;

typedef struct {
	int num_bloque;
	char nombre_nodo_1[NOMBRE_NODO_SIZE];
	int num_bloque_1;
	char nombre_nodo_2[NOMBRE_NODO_SIZE];
	int num_bloque_2;
	int tamanio;
} detalle_archivo_t;

typedef struct {
	int num_bloque;
	char nombre_nodo[NOMBRE_NODO_SIZE];
	int tamanio;
} detalle_archivo_seleccionado_t;

bool transformacion_iniciar(packet_t *packet, socket_t sockMaster, socket_t sockFS, yama_t* config, t_list *estados_master, t_list *nodos);

#endif /* YAMA_SRC_TRANSFORMACION_H_ */
