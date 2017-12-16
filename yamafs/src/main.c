#include "main.h"

yamafs_t *config;
bool p_clean;
bool esperar_DNs;
char comando_global[80];

void validar_parametros(int argc, char **argv) {
	if(!(argc == 1 || (argc == 2 && string_equals_ignore_case(argv[1], "--clean")))) {
		log_msg_error("Debe indicar ninguno o el parametro --clean");
		exit(EXIT_FAILURE);
	}
	p_clean = argc == 2;
}

bool tiene_estructura_de_datos() {
	return directorio_existe_config(config) && nodo_existe_config(config);
}

void borrar_estructura_de_datos() {
	log_msg_info("Borrado de estructuras de dato, formateo del filesystem");
	global_delete_directory(config->metadata_path);
}

void cargar_estructura_de_datos() {
	log_msg_info("Recuperacion del estado anterior, restauracion del filesystem");
	directorio_cargar(config);
	nodo_cargar(config);
}

void crear_estructura_de_datos() {
	log_msg_info("Inicializacion de estructuras de dato, creacion del filesystem");
	if(!global_get_dir_exist(config->metadata_path))
		global_create_dir(config->metadata_path);
	directorio_crear(config);
	nodo_crear(config);
}

void inicializar_estructuras() {
	esperar_DNs = false;
	if(p_clean) {
		//eliminar estado anterior
		borrar_estructura_de_datos();
	}
	if(tiene_estructura_de_datos()) {
		//esperar DNs
		esperar_DNs = true;
		cargar_estructura_de_datos();
	}
	else {
		//inicializar en estado no-estable
		crear_estructura_de_datos();
	}
}

void finalizar() {
	directorio_destruir();
	nodo_destruir();
}

int main(int argc, char **argv) {
	global_set_process(FS);
	validar_parametros(argc, argv);
	config = config_leer("metadata");
	log_init(config->log_file, config->log_name, false);

	inicializar_estructuras();

	pthread_t th_srv = server_crear_fs(config, esperar_DNs);
	consola_iniciar(config, th_srv);

	server_liberar();
	finalizar();
	config_liberar(config);

	printf("Finalizado con exito");

	return EXIT_SUCCESS;
}
