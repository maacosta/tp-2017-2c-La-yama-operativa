#ifndef SRC_OPERATION_H_
#define SRC_OPERATION_H_

#include <stdbool.h>
#include "../../common/global.h"
#include "../../common/socket.h"
#include "../../common/protocol.h"
#include "configuration.h"

void operation_init(yama_t* config, socket_t sockyama, socket_t sockfs);

bool operation_iniciar_tarea(packet_t *packet);

#endif /* SRC_OPERATION_H_ */
