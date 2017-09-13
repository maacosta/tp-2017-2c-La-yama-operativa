#ifndef YAMA_CONFIGURATION_H_
#define YAMA_CONFIGURATION_H_

#include <commons/config.h>
#include <unistd.h>
#include <stdlib.h>
#include "../../common/log.h"
#include "../../common/global.h"

// Configuracion
typedef struct{
	char* log_file;
	char* log_name;
	char* puerto;
	char* yamafs_ip;
	char* yamafs_puerto;
	int retardo_planificacion;
	char* algoritmo_balanceo;
} yama_t;

yama_t *config_leer(const char* path);

void config_liberar(yama_t* config);

#endif /* YAMA_CONFIGURATION_H_ */
