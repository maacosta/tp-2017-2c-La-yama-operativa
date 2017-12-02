#ifndef SRC_SERVER_H_
#define SRC_SERVER_H_

#include <stdbool.h>
#include <commons/collections/list.h>
#include "../../common/global.h"
#include "../../common/socket.h"
#include "../../common/protocol.h"
#include "configuration.h"
#include "operaciones.h"

void server_crear(yamaworker_t *config);

void server_liberar();

#endif /* SRC_SERVER_H_ */
