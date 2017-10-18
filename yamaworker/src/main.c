#include "main.h"



int main(int argc, char **argv) {
	global_set_process(WORKER);
	/*Al iniciar leerá el archivo de configuración del nodo y
    quedará a la espera de conexiones por parte de procesos Master.*/
	yamaworker_t *config;

	config = config_leer("metadata");

	log_init(config->log_file, config->log_name, true);

	socket_t sockMs=0;
	socket_t sockMaster;
	//Cuando se reciba por socket
	//hay que verificar que el script no este bloqueado por los permisos
	//agregarle chmod +x script_transformacion.py

	sockMaster = escuchar_master(config, sockMs);




	return EXIT_SUCCESS;
}
