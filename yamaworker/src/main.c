#include "main.h"

yamaworker_t *config;

int main(int argc, char **argv) {
	global_set_process(WORKER);
	config = config_leer("metadata");
	log_init(config->log_file, config->log_name, true);

	//Cuando se reciba por socket
	//hay que verificar que el script no este bloqueado por los permisos
	//agregarle chmod +x script_transformacion.py
	server_crear(config);
	//fork
	//fork hijo escucha las operaciones de master
		//procesar_operaciones(sockMaster);
	//fork padre no hace nada y continua escuchando a master

	return EXIT_SUCCESS;
}
