/*
 * configuration.c
 *
 *  Created on: 28/9/2017
 *      Author: utnso
 */

#include "configuration.h"

static t_config* c;

yamaDataNodo_t *config_leer(const char* path) {
	if(!global_get_file_exist(path)) {
		exit(EXIT_FAILURE);
	}

	c = config_create((char *)path);
	yamaDataNodo_t *config = malloc(sizeof(yamaDataNodo_t));

	config->ip_fs = config_get_string_value(c, "IP_FS");
	config->puerto_fs = config_get_string_value(c, "PUERTO_FS");
	config->nombreNodo = config_get_string_value(c, "NOMBRE_NODO");
	config->ruta_databin = config_get_string_value(c, "RUTA_DATABIN");
	config->log_file = config_get_string_value(c, "LOG_FILE");


	return config;
}

void config_liberar(yamaDataNodo_t* config) {
	config_destroy(c);
	free(config);
}

