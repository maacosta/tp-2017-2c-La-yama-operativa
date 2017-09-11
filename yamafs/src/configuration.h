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
	int puerto;
} t_yamafs;

t_yamafs *config_leer(const char* path);

void config_liberar(t_yamafs* config);

#endif /* YAMAFS_CONFIGURATION_H_ */
