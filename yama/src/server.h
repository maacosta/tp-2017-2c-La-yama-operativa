#ifndef SRC_SERVER_H_
#define SRC_SERVER_H_

#include <stdbool.h>
#include <limits.h>
#include <commons/collections/list.h>
#include "../../common/global.h"
#include "../../common/socket.h"
#include "../../common/protocol.h"
#include "estados_master.h"
#include "configuration.h"
#include "op_almacenamiento.h"
#include "op_jobs_estados_master.h"
#include "op_reduccion.h"
#include "op_transformacion.h"


void server_crear_yama(yama_t *config, socket_t sockfs);

void server_liberar();

#endif /* SRC_SERVER_H_ */
