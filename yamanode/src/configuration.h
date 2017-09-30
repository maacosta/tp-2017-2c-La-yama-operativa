/*
 * configuration.h
 *
 *  Created on: 28/9/2017
 *      Author: utnso
 */

#ifndef SRC_CONFIGURATION_H_
#define SRC_CONFIGURATION_H_

#include <commons/config.h>
#include <unistd.h>
#include <stdlib.h>
#include "../../common/log.h"
#include "../../common/global.h"

typedef struct{
	char* ip_fs;
	char* puerto_fs;
	char* nombreNodo;
	char* ruta_databin;
	char* log_file;

} yamaDataNodo_t;


yamaDataNodo_t  *config_leer(const char* path);
#endif /* SRC_CONFIGURATION_H_ */
