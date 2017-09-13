#include "configuration.h"

static t_config* c;

yama_t *config_leer(const char* path) {
	if(!global_get_file_exist(path)) {
		exit(EXIT_FAILURE);
	}

	c = config_create((char *)path);
	yama_t *config = malloc(sizeof(yama_t));

	config->log_file = config_get_string_value(c, "LOG_FILE");
	config->log_name = config_get_string_value(c, "LOG_NAME");
	config->puerto = config_get_string_value(c, "PUERTO");
	config->yamafs_ip = config_get_string_value(c, "FS_IP");
	config->yamafs_puerto = config_get_string_value(c, "FS_PUERTO");
	config->retardo_planificacion = config_get_int_value(c, "RETARDO_PLANIFICACION");
	config->algoritmo_balanceo= config_get_string_value(c, "ALGORITMO_BALANCEO");

	return config;
}

void config_liberar(yama_t* config) {
	config_destroy(c);
	free(config);
}

