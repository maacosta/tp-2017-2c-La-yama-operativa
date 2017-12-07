#ifndef SRC_WORKER_TEST_H_
#define SRC_WORKER_TEST_H_

#include <stdbool.h>
#include "../../common/socket.h"
#include "../../common/protocol.h"

void worker_enviar_transformacion(socket_t sockWorker);
void worker_enviar_reduccion(socket_t sockWorker);

#endif /* SRC_WORKER_TEST_H_ */
