/*
 * yamanode.h
 *
 *  Created on: 4/9/2017
 *      Author: utnso
 */

#ifndef SRC_YAMADATANODO_H_
#define SRC_YAMADATANODO_H_

#include <pthread.h>
#include <unistd.h>
#include <libio.h>
#include "../../common/protocol.h"
#include "configuration.h"



int rcx;

typedef struct {
	int sock_fs;
} t_conexion_nodo;


#endif /* SRC_YAMADATANODO_H_ */
