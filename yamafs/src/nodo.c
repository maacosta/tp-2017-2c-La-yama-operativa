#include "nodo.h"

#define KEY_TAMANIO "TAMANIO"
#define KEY_LIBRE "LIBRE"
#define KEY_NODOS "NODOS"
#define KEY_NODO_TOTAL "TOTAL"
#define KEY_NODO_LIBRE "LIBRE"

t_config *nodo_config;
/* Solo para cuando va por nodo_cargar(...)
 */
int nodos_registrados;
/* Solo para cuando va por nodo_cargar(...)
 */
int nodos_registrados_totales;

bool nodo_existe_config(yamafs_t *config) {
	char *path = string_from_format("%s/nodos.dat", config->metadata_path);
	return access(path, F_OK) != -1;
}

void nodo_borrar(yamafs_t *config) {
	char *path = string_from_format("%s/nodos.dat", config->metadata_path);
	global_delete_file(path);
	free(path);
}

void nodo_crear(yamafs_t *config) {
	char *path = string_from_format("%s/nodos.dat", config->metadata_path);

	if(!global_create_txtfile(path, NULL, 0)) exit(EXIT_FAILURE);

	nodo_config = config_create(path);

	config_set_value(nodo_config, KEY_NODOS, "");
	config_set_value(nodo_config, KEY_TAMANIO, "0");
	config_set_value(nodo_config, KEY_LIBRE, "0");

	nodos_registrados_totales = nodos_registrados = 0;

	config_save(nodo_config);

	free(path);
}

void nodo_cargar(yamafs_t *config) {
	char *path = string_from_format("%s/nodos.dat", config->metadata_path);

	nodo_config = config_create(path);

	char *nodos = config_get_string_value(nodo_config, KEY_NODOS);
	char **nodo_lista = string_split(nodos, "|");

	bool existe = false;
	nodos_registrados_totales = nodos_registrados = 0;
	void iterar(char *d) {
		nodos_registrados_totales++;
	}
	string_iterate_lines(nodo_lista, (void*)iterar);
}

bool nodo_todos_registrados() {
	return nodos_registrados_totales == nodos_registrados;
}

bool nodo_existe(datos_nodo_registro_t *nodo) {
	bool r;
	char *key = string_from_format("%s%s", nodo->nombre_nodo, KEY_NODO_TOTAL);
	r = config_has_property(nodo_config, key);
	free(key);
	return r;
}

void nodo_notificar_existencia(datos_nodo_registro_t *nodo) {
	nodos_registrados++;
}

void nodo_agregar(datos_nodo_registro_t *nodo) {
	char *key;
	int tamanio = config_get_int_value(nodo_config, KEY_TAMANIO);
	int libre = config_get_int_value(nodo_config, KEY_LIBRE);
	char *nodos = config_get_string_value(nodo_config, KEY_NODOS);
	char **nodo_lista = string_split(nodos, "|");
	char *n;

	bool existe = false;
	int c = 0;
	void iterar(char *d) {
		if(string_equals_ignore_case(d, nodo->nombre_nodo)) existe = true;
		c++;
	}
	string_iterate_lines(nodo_lista, (void*)iterar);

	if(!existe) {
		key = string_from_format("%s%s", nodo->nombre_nodo, KEY_NODO_TOTAL);
		config_set_value(nodo_config, key, string_itoa(nodo->cantidad_bloques));
		free(key);
		key = string_from_format("%s%s", nodo->nombre_nodo, KEY_NODO_LIBRE);
		config_set_value(nodo_config, key, string_itoa(nodo->cantidad_bloques));
		free(key);
		if(c != 0) n = string_from_format("%s|%s", nodos, nodo->nombre_nodo);
		else n = string_from_format("%s", nodos, nodo->nombre_nodo);
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
