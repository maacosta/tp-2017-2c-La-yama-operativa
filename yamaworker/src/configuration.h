#ifndef YAMAWORKER_CONFIGURATION_H_
#define YAMAWORKER_CONFIGURATION_H_

#include <commons/config.h>
#include <unistd.h>
#include <stdlib.h>
#include "../../common/log.h"
#include "../../common/global.h"

// Configuracion
typedef struct {
	char* log_file;
	char* log_name;
	char* nombre_nodo;
	char* puerto_nodo;
	char* yamafs_ip;
	char* yamafs_puerto;
	char* path_tmp;
	char* path_databin;
} yamaworker_t;

yamaworker_t *config_leer(const char* path);

void config_liberar(yamaworker_t* config);

#endif /* YAMAWORKER_CONFIGURATION_H_ */
