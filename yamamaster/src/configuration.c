#include "configuration.h"

static t_config* c;

yamamaster_t *config_leer(const char* path) {
	if(!global_get_file_exist(path)) {
		exit(EXIT_FAILURE);
	}

	c = config_create((char *)path);
	yamamaster_t *config = malloc(sizeof(yamamaster_t));

	config->log_file = config_get_string_value(c, "LOG_FILE");
	config->log_name = config_get_string_value(c, "LOG_NAME");
	config->yama_ip = config_get_string_value(c, "YAMA_IP");
	config->yama_puerto = config_get_string_value(c, "YAMA_PUERTO");

	return config;
}

void config_liberar(yamamaster_t* config) {
	config_destroy(c);
	free(config);
}

