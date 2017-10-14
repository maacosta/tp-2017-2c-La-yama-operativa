#ifndef SRC_ALMACENAMIENTO_H_
#define SRC_ALMACENAMIENTO_H_

#include "../../common/thread.h"
#include "../../common/protocol.h"
#include "../../common/socket.h"

void ejecutar_almacenamiento(socket_t sockYama, char *archivo_destino, char *archivo_origen);

#endif /* SRC_ALMACENAMIENTO_H_ */
