#ifndef SRC_SERVER_H_
#define SRC_SERVER_H_

#include <stdbool.h>
#include <commons/collections/list.h>
#include "../../common/global.h"
#include "../../common/socket.h"
#include "../../common/protocol.h"
#include "configuration.h"
#include "transformacion.h"

typedef enum {
	ETAPA_Transformacion,
	ETAPA_Reduccion_Local,
	ETAPA_Reduccion_Global,
	ETAPA_Almacenamiento_Final
} etapa_t;

typedef enum {
	ESTADO_En_Proceso,
	ESTADO_Finalizado_OK,
	ESTADO_Error
} estado_t;

typedef struct {
	int job;
	socket_t master;
	char nodo[NOMBRE_NODO_SIZE];
	int bloque;
	etapa_t etapa;
	char archivo_temporal[NOMBRE_ARCHIVO_TMP];
	estado_t estado;
} estado_master_t;

void server_crear(yama_t *config, socket_t sockfs, t_list *nodos);

void server_crear_nombre_archivo_temporal(char *nombre);

void server_liberar();

#endif /* SRC_SERVER_H_ */
