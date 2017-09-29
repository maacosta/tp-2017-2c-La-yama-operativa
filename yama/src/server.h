#ifndef SRC_SERVER_H_
#define SRC_SERVER_H_

#include <stdbool.h>
#include "../../common/global.h"
#include "../../common/socket.h"
#include "../../common/protocol.h"
#include "configuration.h"
#include "operation.h"

void server_create(yama_t *config, socket_t sockfs);

#endif /* SRC_SERVER_H_ */
