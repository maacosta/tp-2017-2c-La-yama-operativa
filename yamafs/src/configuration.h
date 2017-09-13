#ifndef YAMAFS_CONFIGURATION_H_
#define YAMAFS_CONFIGURATION_H_

#include <commons/config.h>
#include <unistd.h>
#include <stdlib.h>
#include "../../common/log.h"

// Configuracion
typedef struct{
	char* log_file;
	char* log_name;
	char* puerto;
} yamafs_t;

yamafs_t *config_leer(const char* path);

void config_liberar(yamafs_t* config);

#endif /* YAMAFS_CONFIGURATION_H_ */
