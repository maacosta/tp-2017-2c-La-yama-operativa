#ifndef SRC_YAMAFS_TEST_H_
#define SRC_YAMAFS_TEST_H_

#include <stdio.h>
#include <stdlib.h>
#include "../../common/socket.h"
#include "../../common/protocol.h"
#include <string.h>

void yamafs_registrar_almacenar_obtener_txtbloque(socket_t sockO, socket_t sockD);
void yamafs_registrar_almacenar_obtener_binbloque(socket_t sockO, socket_t sockD);

#endif /* SRC_YAMAFS_TEST_H_ */
