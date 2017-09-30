#include "main.h"

yamafs_t *config;

int main(int argc, char **argv) {
	global_set_process(FS);
	config = config_leer("metadata");
	log_init(config->log_file, config->log_name, false);

	consola_crear(config);
	server_crear(config);
	config_liberar(config);

	return EXIT_SUCCESS;
}
