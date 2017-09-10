#include "main.h"
#include "configuration.h"

t_yamafs *config;

int main(int argc, char **argv) {
	//config = config_leer("../metadata");
	config = config_leer(argv[1]);

	log_init(config->log_file, config->log_name, false);

	config_liberar(config);

	pthread_t th_consola;
	pthread_create(&th_consola, NULL, (void *) consola_iniciar, NULL);

	pthread_join(th_consola, NULL);

	return 0;
}
