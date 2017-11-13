#include "nodo.h"

#define KEY_TAMANIO "TAMANIO"
#define KEY_LIBRE "LIBRE"
#define KEY_NODOS "NODOS"
#define KEY_NODO_TOTAL "TOTAL"
#define KEY_NODO_LIBRE "LIBRE"

t_config *nodo_config;

bool nodo_existe_config(yamafs_t *config) {
	char *path = string_from_format("%s/nodos.dat", config->metadata_path);
	return access(path, F_OK) != -1;
}

void nodo_borrar(yamafs_t *config) {
	char *path = string_from_format("%s/nodos.dat", config->metadata_path);
	global_deletefile(path);
	free(path);
}

void nodo_crear(yamafs_t *config) {
	char *path = string_from_format("%s/nodos.dat", config->metadata_path);

	if(!global_createfile(path)) exit(EXIT_FAILURE);

	nodo_config = config_create(path);

	config_set_value(nodo_config, KEY_NODOS, "");
	config_set_value(nodo_config, KEY_TAMANIO, "0");
	config_set_value(nodo_config, KEY_LIBRE, "0");

	config_save(nodo_config);

	free(path);
}

void nodo_cargar(yamafs_t *config) {
	char *path = string_from_format("%s/nodos.dat", config->metadata_path);

	nodo_config = config_create(path);
}

void nodo_agregar(datos_nodo_registro_t *nodo) {
	char *key;
	int tamanio = config_get_int_value(nodo_config, KEY_TAMANIO);
	int libre = config_get_int_value(nodo_config, KEY_LIBRE);
	char *nodos = config_get_string_value(nodo_config, KEY_NODOS);
	char **nodo_lista = string_split(nodos, "|");
	char *n;

	bool existe = false;
	void iterar(char *d) {
		if(string_equals_ignore_case(d, nodo->nombre_nodo)) existe = true;
	}
	string_iterate_lines(nodo_lista, (void*)iterar);

	if(!existe) {
		key = string_from_format("%s%s", nodo->nombre_nodo, KEY_NODO_TOTAL);
		config_set_value(nodo_config, key, string_itoa(nodo->cantidad_bloques));
		free(key);
		key = string_from_format("%s%s", nodo->nombre_nodo, KEY_NODO_LIBRE);
		config_set_value(nodo_config, key, string_itoa(nodo->cantidad_bloques));
		free(key);
		n = string_from_format("%s|%s", nodos, nodo->nombre_nodo);
		config_set_value(nodo_config, KEY_NODOS, n);
		config_set_value(nodo_config, KEY_TAMANIO, string_itoa(tamanio + nodo->cantidad_bloques));
		config_set_value(nodo_config, KEY_LIBRE, string_itoa(libre + nodo->cantidad_bloques));
		free(n);

		config_save(nodo_config);
	}
}

void nodo_quitar(datos_nodo_registro_t *nodo) {
}

void nodo_destruir() {
	config_destroy(nodo_config);
}
