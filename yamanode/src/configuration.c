#include "configuration.h"

static t_config* c;

yamaDataNodo_t *config_leer(const char* path) {
	if(!global_get_file_exist(path)) {
		exit(EXIT_FAILURE);
	}

	c = config_create((char *)path);
	yamaDataNodo_t *config = malloc(sizeof(yamaDataNodo_t));

	config->yamafs_ip = config_get_string_value(c, "YAMAFS_IP");
	config->yamafs_puerto = config_get_string_value(c, "YAMAFS_PUERTO");
	config->nombre_nodo = config_get_string_value(c, "NOMBRE_NODO");
	config->ip_nodo = config_get_string_value(c, "IP_NODO");
	config->puerto_nodo = config_get_string_value(c, "PUERTO_NODO");
	config->path_databin = config_get_string_value(c, "PATH_DATABIN");
	config->log_file = config_get_string_value(c, "LOG_FILE");
	config->log_name = config_get_string_value(c, "LOG_NAME");

	return config;
}

void config_liberar(yamaDataNodo_t* config) {
	config_destroy(c);
	free(config);
}

