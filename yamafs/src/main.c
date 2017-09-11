#include "main.h"

t_yamafs *config;

int main(int argc, char **argv) {
	config = config_leer("metadata");

	log_init(config->log_file, config->log_name, false);

	consola_crear(config);

	config_liberar(config);

	return 0;
}
