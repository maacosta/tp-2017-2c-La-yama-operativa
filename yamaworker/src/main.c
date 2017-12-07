#include "main.h"

yamaworker_t *config;

int main(int argc, char **argv) {
	global_set_process(WORKER);
	config = config_leer("metadata");
	log_init(config->log_file, config->log_name, true);

	server_crear(config);

	return EXIT_SUCCESS;
}
