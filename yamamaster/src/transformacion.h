#ifndef SRC_TRANSFORMACION_H_
#define SRC_TRANSFORMACION_H_

#include "../../common/protocol.h"
#include "../../common/socket.h"

void ejecutar_transformacion(socket_t sock, char *archivo_transformador, char *archivo_origen);

#endif /* SRC_TRANSFORMACION_H_ */
