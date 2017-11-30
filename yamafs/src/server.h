#ifndef SRC_SERVER_H_
#define SRC_SERVER_H_

#include <stdbool.h>
#include <limits.h>
#include <sys/signalfd.h>
#include <commons/collections/list.h>
#include "../../common/global.h"
#include "../../common/socket.h"
#include "../../common/protocol.h"
#include "../../common/thread.h"
#include "configuration.h"
#include "op_nodos.h"

pthread_t server_crear_fs(yamafs_t *config, bool esperarDNs);

void server_liberar();

#endif /* SRC_SERVER_H_ */
