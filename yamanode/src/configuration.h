#ifndef SRC_CONFIGURATION_H_
#define SRC_CONFIGURATION_H_

#include <commons/config.h>
#include <unistd.h>
#include <stdlib.h>
#include "../../common/log.h"
#include "../../common/global.h"

typedef struct{
	char* log_file;
	char* log_name;
	char* nombre_nodo;
	char* ip_nodo;
	char* puerto_nodo;
	char* yamafs_ip;
	char* yamafs_puerto;
	char* path_databin;
} yamaDataNodo_t;

yamaDataNodo_t  *config_leer(const char* path);
void config_liberar(yamaDataNodo_t* config);

#endif /* SRC_CONFIGURATION_H_ */
