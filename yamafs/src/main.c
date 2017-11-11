#include "main.h"

yamafs_t *config;
bool p_clean;
bool esperar_DNs;

void validar_parametros(int argc, char **argv) {
	if(!(argc == 1 || (argc == 2 && string_equals_ignore_case(argv[1], "--clean")))) {
		log_msg_error("Debe indicar ninguno o el parametro --clean");
		exit(EXIT_FAILURE);
	}
	p_clean = argc == 2;
	log_msg_info("Parametros %s", argv[1]);
}

bool tiene_estructura_de_datos() {
	return directorio_existe_config(config);
}

void borrar_estructura_de_datos() {
	directorio_borrar(config);
}

void cargar_estructura_de_datos() {
	directorio_cargar(config);
}

void crear_estructura_de_datos() {
	directorio_crear(config);
}

void inicializar() {
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
}

int main(int argc, char **argv) {
	global_set_process(FS);
	validar_parametros(argc, argv);
	config = config_leer("metadata");
	log_init(config->log_file, config->log_name, false);

	inicializar();

	server_crear_fs(config, esperar_DNs);
	consola_iniciar(config);

	finalizar();

	config_liberar(config);

	return EXIT_SUCCESS;
}
