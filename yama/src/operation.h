#ifndef SRC_OPERATION_H_
#define SRC_OPERATION_H_

#include <stdbool.h>
#include <commons/collections/list.h>
#include "../../common/global.h"
#include "../../common/socket.h"
#include "../../common/protocol.h"
#include "configuration.h"
#include "server.h"

void operation_init(yama_t* config, socket_t sockyama, socket_t sockfs);

bool operation_iniciar_tarea(packet_t *packet, t_list *estados_master, socket_t cliente);

#endif /* SRC_OPERATION_H_ */
