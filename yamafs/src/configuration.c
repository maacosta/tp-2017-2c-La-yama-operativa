#include "configuration.h"

yamafs_t *config_leer(const char* path) {
	if(!global_get_file_exist(path)) {
		exit(EXIT_FAILURE);
	}

	t_config* c = config_create((char *)path);
	yamafs_t *config = malloc(sizeof(yamafs_t));

	config->log_file = config_get_string_value(c, "LOG_FILE");
	config->log_name = config_get_string_value(c, "LOG_NAME");
	config->puerto = config_get_string_value(c, "PUERTO");
	config->metadata_path = config_get_string_value(c, "METADATA_PATH");

	return config;
}

void config_liberar(yamafs_t* config) {
	free(config);
}
