#ifndef YAMAMASTER_CONFIGURATION_H_
#define YAMAMASTER_CONFIGURATION_H_

#include <commons/config.h>
#include <unistd.h>
#include <stdlib.h>
#include "../../common/log.h"
#include "../../common/global.h"

// Configuracion
typedef struct{
	char* log_file;
	char* log_name;
	char* yama_ip;
	char* yama_puerto;
} yamamaster_t;

yamamaster_t *config_leer(const char* path);

void config_liberar(yamamaster_t* config);

#endif /* YAMAMASTER_CONFIGURATION_H_ */
