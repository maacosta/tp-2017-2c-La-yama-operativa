#ifndef SRC_SERVER_H_
#define SRC_SERVER_H_

#include <stdbool.h>
#include <limits.h>
#include <commons/collections/list.h>
#include "../../common/global.h"
#include "../../common/socket.h"
#include "../../common/protocol.h"
#include "configuration.h"
#include "jobs_estados_master.h"
#include "transformacion.h"
#include "reduccion.h"
#include "almacenamiento.h"


void server_crear_yama(yama_t *config, socket_t sockfs, t_list *nodos);

void server_crear_nombre_archivo_temporal(char *nombre);

void server_liberar();

#endif /* SRC_SERVER_H_ */
