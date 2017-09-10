#include "main.h"
#include "configuration.h"

t_yamafs *config;

int main(int argc, char **argv) {
	//config = config_leer("metadata");
	config = config_leer(argv[1]);

	log_init(config->log_file, config->log_name, false);

	config_liberar(config);

	return 0;
}
