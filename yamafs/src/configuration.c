/*
 * configuration.c
 *
 *  Created on: 9/9/2017
 *      Author: utnso
 */

#include "configuration.h"

static t_config* c;

yamafs_t *config_leer(const char* path) {
	if(!global_get_file_exist(path)) {
		exit(EXIT_FAILURE);
	}

	c = config_create((char *)path);
	yamafs_t *config = malloc(sizeof(yamafs_t));

	config->log_file = config_get_string_value(c, "LOG_FILE");
	config->log_name = config_get_string_value(c, "LOG_NAME");
	config->puerto = config_get_string_value(c, "PUERTO");

	return config;
}

void config_liberar(yamafs_t* config) {
	config_destroy(c);
	free(config);
}
