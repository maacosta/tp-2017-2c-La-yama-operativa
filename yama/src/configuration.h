/*
 * configuration.h
 *
 *  Created on: 11/9/2017
 *      Author: utnso
 */

#ifndef YAMA_CONFIGURATION_H_
#define YAMA_CONFIGURATION_H_

#include <commons/config.h>
#include <unistd.h>
#include <stdlib.h>
#include "../../common/log.h"

// Configuracion
typedef struct{
	char* yama_ip;
	int yama_puerto;
	int yama_retardo_planificacion;
	char* yama_algoritmo_balanceo;
	char* log_file;
	char* log_name;
} yama_t;


yama_t *config_leer(const char* path);

void config_liberar(yama_t* config);

#endif /* SRC_CONFIGURATION_H_ */
