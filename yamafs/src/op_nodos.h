#ifndef SRC_OP_NODOS_H_
#define SRC_OP_NODOS_H_

#include <stdbool.h>
#include <commons/collections/list.h>
#include "../../common/global.h"
#include "../../common/socket.h"
#include "../../common/protocol.h"
#include "nodo.h"

bool nodos_registrar(packet_t *packet, socket_t sockDN, bool *esperarDNs, bool *estadoEstable);

#endif /* SRC_OP_NODOS_H_ */
