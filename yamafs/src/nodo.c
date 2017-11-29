#include "nodo.h"

#define KEY_TAMANIO "TAMANIO"
#define KEY_LIBRE "LIBRE"
#define KEY_NODOS "NODOS"
#define KEY_NODO_TOTAL "TOTAL"
#define KEY_NODO_LIBRE "LIBRE"

t_config *nodo_config;
t_list *nodos_registrados_lista;
t_list *nodos_lista;

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

	config_set_value(nodo_config, KEY_NODOS, "[]");
	config_set_value(nodo_config, KEY_TAMANIO, "0");
	config_set_value(nodo_config, KEY_LIBRE, "0");

	nodos_lista = list_create();
	nodos_registrados_lista = list_create();

	config_save(nodo_config);
	free(path);
}

void nodo_cargar(yamafs_t *config) {
	char *path = string_from_format("%s/nodos.dat", config->metadata_path);
	nodo_config = config_create(path);

	nodos_lista = list_create();
	nodos_registrados_lista = list_create();

	char **nodos = config_get_array_value(nodo_config, KEY_NODOS);

	void iterar(char *n) {
		list_add(nodos_lista, n);
	}
	string_iterate_lines(nodos, (void*)iterar);
	free(nodos);
}

bool nodo_existe(const char *nombre_nodo) {
	bool existe;
	char **nodos = config_get_array_value(nodo_config, KEY_NODOS);

	void iterar(char *n) {
		if(string_equals_ignore_case(n, (char *)nombre_nodo)) existe = true;
	}
	string_iterate_lines(nodos, (void*)iterar);
	free(nodos);
	return existe;
}

void nodo_notificar_existencia(const char *nombre_nodo) {
	void iterar(char *n) {
		if(string_equals_ignore_case(n, (char *)nombre_nodo))
			list_add(nodos_registrados_lista, (void *)nombre_nodo);
	}
	list_iterate(nodos_lista, (void *)iterar);
}

bool nodo_se_notificaron_todos_los_registrados() {
	bool todos_registrados = true;
	void iterar(char *n) {
		bool buscar(char *nn) {
			return string_equals_ignore_case(n, nn);
		}
		if(!list_any_satisfy(nodos_registrados_lista, (void *)buscar))
			todos_registrados = false;;
	}
	list_iterate(nodos_lista, (void *)iterar);
	return todos_registrados;
}

static void agregar_nombre_nodo_en_config(const char *nombre_nodo) {
	if (nodo_existe(nombre_nodo)) return;

	int c = 0;
	char **nodos = config_get_array_value(nodo_config, KEY_NODOS);
	void iterar_contar(char *n) { c++; }
	string_iterate_lines(nodos, (void*)iterar_contar);

	char *nodos_string;
	if(c == 0)
		nodos_string = string_from_format("[%s]", nombre_nodo);
	else {
		char *nn = string_from_format("%s", nombre_nodo);
		void iterar(char *n) {
			string_append_with_format(&nn, ",%s", n);
		}
		string_iterate_lines(nodos, (void*)iterar);
		nodos_string = string_from_format("[%s]", nn);
		free(nn);
	}
	config_set_value(nodo_config, KEY_NODOS, nodos_string);
}

static void quitar_nombre_nodo_en_config(char *nombre_nodo) {
	if (!nodo_existe(nombre_nodo)) return;

	int c = 0;
	char **nodos = config_get_array_value(nodo_config, KEY_NODOS);
	void iterar_contar(char *n) { c++; }
	string_iterate_lines(nodos, (void*)iterar_contar);

	char *nodos_string;
	if (c == 1){
		nodos_string = string_from_format("[]");
	}
	else {
		char *nn = string_new();
		void iterar(char *n) {
			if (!string_equals_ignore_case(n, nombre_nodo))
				string_append_with_format(&nn, (c > 1 ? "%s," : "%s"), n);
			c--;
		}
		string_iterate_lines(nodos, (void*)iterar);
		nodos_string = string_from_format("[%s]", nn);
		free(nn);
	}
	config_set_value(nodo_config, KEY_NODOS, nodos_string);
}

static void actualizar_bloques_en_config(int cant_bloques_totales, int cant_bloques_libres) {
	int tamanio = config_get_int_value(nodo_config, KEY_TAMANIO);
	int libre = config_get_int_value(nodo_config, KEY_LIBRE);
	config_set_value(nodo_config, KEY_TAMANIO, string_itoa(tamanio + cant_bloques_totales));
	config_set_value(nodo_config, KEY_LIBRE, string_itoa(libre + cant_bloques_libres));
}

void nodo_agregar(const char *nombre_nodo, int cant_bloques_totales, int cant_bloques_libres) {
	if (nodo_existe(nombre_nodo)) return;

	char *key = string_from_format("%s%s", nombre_nodo, KEY_NODO_TOTAL);
	config_set_value(nodo_config, key, cant_bloques_totales);
	free(key);
	key = string_from_format("%s%s", nombre_nodo, KEY_NODO_LIBRE);
	config_set_value(nodo_config, key, cant_bloques_libres);
	free(key);

	agregar_nombre_nodo_en_config(nombre_nodo);
	actualizar_bloques_en_config(cant_bloques_totales, cant_bloques_libres);

	config_save(nodo_config);
}

void nodo_actualizar(const char *nombre_nodo, int cant_bloques_libres) {
	if (!nodo_existe(nombre_nodo)) return;

	char *key = string_from_format("%s%s", nombre_nodo, KEY_NODO_LIBRE);
	int libre = config_get_int_value(nodo_config, key);
	config_set_value(nodo_config, key, cant_bloques_libres);
	free(key);

	actualizar_bloques_en_config(0, cant_bloques_libres - libre);

	config_save(nodo_config);
}

void nodo_quitar(const char *nombre_nodo) {
	if (!nodo_existe(nombre_nodo)) return;

	char *key = string_from_format("%s%s", nombre_nodo, KEY_NODO_TOTAL);
	int total = config_get_int_value(nodo_config, key);
	free(key);
	key = string_from_format("%s%s", nombre_nodo, KEY_NODO_LIBRE);
	int libre = config_get_int_value(nodo_config, key);
	free(key);

	quitar_nombre_nodo_en_config((char *)nombre_nodo);
	actualizar_bloques_en_config(total * -1, libre * -1);

	config_save(nodo_config);
}

void nodo_obtener(const char *nombre_nodo, int *cant_bloques_totales, int *cant_bloques_libres) {
	if (!nodo_existe(nombre_nodo)) return;

	char *key = string_from_format("%s%s", nombre_nodo, KEY_NODO_TOTAL);
	*cant_bloques_totales = config_get_int_value(nodo_config, key);
	free(key);
	key = string_from_format("%s%s", nombre_nodo, KEY_NODO_LIBRE);
	*cant_bloques_libres = config_get_int_value(nodo_config, key);
	free(key);
}

char *nodo_obtener_rnd(int *cant_bloques_totales, int *cant_bloques_libres) {
	char **nodos = nodo_lista_nombre();
	char *nombre_nodo;
	int bloques_libres = nodo_obtener_bloques_libres();
	int rnd = global_rnd(0, bloques_libres);

	int libres_acumulados = 0;
	bool encontrado = false;
	void iterar_nodos(char *n) {
		int total, libres;
		nodo_obtener(n, &total, &libres);
		libres_acumulados += libres;
		if(rnd < libres_acumulados && !encontrado) {
			nombre_nodo = n;
			*cant_bloques_totales = total;
			*cant_bloques_libres = libres;
			encontrado = true;
		}
	}
	string_iterate_lines(nodos, (void *)iterar_nodos);
	free(nodos);
	return nombre_nodo;
}

int nodo_cantidad() {
	int c = 0;
	char **nodos = config_get_array_value(nodo_config, KEY_NODOS);
	void iterar_contar(char *n) { c++; }
	string_iterate_lines(nodos, (void*)iterar_contar);
	free(nodos);
	return c;
}

char **nodo_lista_nombre() {
	return config_get_array_value(nodo_config, KEY_NODOS);
}

int nodo_obtener_bloques_libres() {
	return config_get_int_value(nodo_config, KEY_LIBRE);
}

int nodo_obtener_bloques_totales() {
	return config_get_int_value(nodo_config, KEY_TAMANIO);
}

void nodo_destruir() {
	config_destroy(nodo_config);
}
