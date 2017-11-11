#ifndef SRC_OP_ALMACENAMIENTO_H_
#define SRC_OP_ALMACENAMIENTO_H_

#include <stdbool.h>
#include <commons/collections/list.h>
#include "../../common/global.h"
#include "../../common/socket.h"
#include "../../common/protocol.h"
#include "configuration.h"
#include "filesystem.h"
#include "server.h"

bool almacenamiento_iniciar(packet_t *packet, socket_t sockMaster, t_list *estados_master, t_list *nodos);

#endif /* SRC_OP_ALMACENAMIENTO_H_ */
