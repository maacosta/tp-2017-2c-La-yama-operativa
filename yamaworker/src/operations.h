/*
 * operations.h
 *
 *  Created on: 18/10/2017
 *      Author: utnso
 */

#ifndef SRC_OPERATIONS_H_
#define SRC_OPERATIONS_H_

#include <sys/wait.h>
#include <stdbool.h>
#include <commons/collections/list.h>
#include "../../common/global.h"
#include "../../common/socket.h"
#include "../../common/protocol.h"
#include "configuration.h"
#include "server.h"

#define SIZE 1024

void operation_init(yamaworker_t* c, socket_t sockworker, socket_t sockms);
#endif /* SRC_OPERATIONS_H_ */
