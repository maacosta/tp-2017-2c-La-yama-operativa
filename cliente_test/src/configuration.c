#include "configuration.h"

static t_config* c;

yamaworker_t *config_leer(const char* path) {
	if(!global_get_file_exist(path)) {
		exit(EXIT_FAILURE);
	}

	c = config_create((char *)path);
	yamaworker_t *config = malloc(sizeof(yamaworker_t));

	config->log_file = config_get_string_value(c, "LOG_FILE");
	config->log_name = config_get_string_value(c, "LOG_NAME");
	config->puerto = config_get_string_value(c, "PUERTO_WORKER");
	config->fs_ip = config_get_string_value(c, "FS_IP");
	config->fs_puerto = config_get_string_value(c, "FS_PUERTO");

	return config;
}

void config_liberar(yamaworker_t* config) {
	config_destroy(c);
	free(config);
}



