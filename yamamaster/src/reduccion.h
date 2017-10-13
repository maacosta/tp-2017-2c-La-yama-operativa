#ifndef SRC_REDUCCION_H_
#define SRC_REDUCCION_H_

#include "../../common/thread.h"
#include "../../common/protocol.h"
#include "../../common/socket.h"

void ejecutar_reduccion(socket_t sockYama, char *archivo_reductor, char *archivo_origen);

#endif /* SRC_REDUCCION_H_ */
