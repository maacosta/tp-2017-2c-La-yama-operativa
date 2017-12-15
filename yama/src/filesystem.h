#ifndef SRC_FILESYSTEM_H_
#define SRC_FILESYSTEM_H_

#include <stdlib.h>
#include <stdbool.h>
#include <commons/collections/list.h>
#include "../../common/global.h"
#include "../../common/socket.h"
#include "../../common/protocol.h"
#include "configuration.h"

void filesystem_obtener_nodos(socket_t sock);

#endif /* SRC_FILESYSTEM_H_ */
