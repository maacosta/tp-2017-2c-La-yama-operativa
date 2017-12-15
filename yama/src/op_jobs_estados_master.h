#ifndef SRC_OP_JOBS_ESTADOS_MASTER_H_
#define SRC_OP_JOBS_ESTADOS_MASTER_H_

#include <stdbool.h>
#include <commons/collections/list.h>
#include "../../common/global.h"
#include "../../common/socket.h"
#include "../../common/protocol.h"

bool jem_consultar(packet_t *packet, socket_t sockMaster);

#endif /* SRC_OP_JOBS_ESTADOS_MASTER_H_ */
