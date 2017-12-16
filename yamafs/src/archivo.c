#include "archivo.h"

#define KEY_BLOQUES "BLOQUES"
#define KEY_TAMANIO "TAMANIO"
#define KEY_TIPO "TIPO"
#define VAL_TIPO_TXT "TEXTO"
#define VAL_TIPO_BIN "BINARIO"
#define KEY_COPIA_0 "COPIA0"
#define KEY_COPIA_1 "COPIA1"
#define KEY_BYTES "BYTES"
#define KEY_BLOQUE "BLOQUE"

bool archivo_existe_config_nombre(yamafs_t *config, const char *nombre_archivo, int indice) {
	char *path = string_from_format("%sarchivos/%d/%s", config->metadata_path, indice, nombre_archivo);
	bool r = access(path, F_OK) != -1;
	free(path);
	return r;
}

void archivo_borrar(yamafs_t *config, const char *nombre_archivo, int indice) {
	char *path = string_from_format("%sarchivos/%d/%s", config->metadata_path, indice, nombre_archivo);
	global_delete_file(path);
	free(path);
}

static void crear_path_archivo(yamafs_t *config, int indice) {
	char *path = string_from_format("%sarchivos", config->metadata_path);
	if(!global_get_dir_exist(path))
		global_create_dir(path);
	free(path);
	path = string_from_format("%sarchivos/%d", config->metadata_path, indice);
	if(!global_get_dir_exist(path))
		global_create_dir(path);
	free(path);
}

t_config *archivo_cargar(yamafs_t *config, const char *nombre_archivo, int indice) {
	char *path = string_from_format("%sarchivos/%d/%s", config->metadata_path, indice, nombre_archivo);

	crear_path_archivo(config, indice);

	if(!archivo_existe_config_nombre(config, nombre_archivo, indice))
		if(!global_create_txtfile(path, NULL, 0))
			exit(EXIT_FAILURE);

	t_config *arc_config = config_create(path);
	free(path);

	return arc_config;
}

void archivo_definir_cabecera(t_config *arc_config, int cant_bloques, int tamanio, bool es_txt) {
	config_set_value(arc_config, KEY_BLOQUES, string_itoa(cant_bloques));
	config_set_value(arc_config, KEY_TAMANIO, string_itoa(tamanio));
	config_set_value(arc_config, KEY_TIPO, es_txt ? VAL_TIPO_TXT : VAL_TIPO_BIN);
	config_save(arc_config);
}

void archivo_recuperar_cabecera(t_config *arc_config, int *cant_bloques, int *tamanio, bool *es_txt) {
	*cant_bloques = config_get_int_value(arc_config, KEY_BLOQUES);
	*tamanio = config_get_int_value(arc_config, KEY_TAMANIO);
	char *v = config_get_string_value(arc_config, KEY_TIPO);
	*es_txt = string_equals_ignore_case(VAL_TIPO_TXT, v);
}

void archivo_definir_copias(t_config *arc_config, int indice, int bytes, char *nombre_nodo_0, int bloque_0, char *nombre_nodo_1, int bloque_1) {
	char *key = string_from_format("%s%d%s", KEY_BLOQUE, indice, KEY_BYTES);
	config_set_value(arc_config, key, string_itoa(bytes));
	free(key);
	key = string_from_format("%s%d%s", KEY_BLOQUE, indice, KEY_COPIA_0);
	config_set_value(arc_config, key, string_from_format("[%s,%d]", nombre_nodo_0, bloque_0));
	free(key);
	key = string_from_format("%s%d%s", KEY_BLOQUE, indice, KEY_COPIA_1);
	config_set_value(arc_config, key, string_from_format("[%s,%d]", nombre_nodo_1, bloque_1));
	free(key);
	config_save(arc_config);
}

void archivo_recuperar_copias(t_config *arc_config, int indice, int *bytes, char *nombre_nodo_0, int *bloque_0, char *nombre_nodo_1, int *bloque_1) {
	char *key = string_from_format("%s%d%s", KEY_BLOQUE, indice, KEY_BYTES);
	*bytes = config_get_int_value(arc_config, key);
	free(key);
	key = string_from_format("%s%d%s", KEY_BLOQUE, indice, KEY_COPIA_0);
	char **pv = config_get_array_value(arc_config, key);
	strcpy(nombre_nodo_0, pv[0]);
	*bloque_0 = atoi(pv[1]);
	free(key);
	string_iterate_lines(pv, (void *)free);
	free(pv);
	key = string_from_format("%s%d%s", KEY_BLOQUE, indice, KEY_COPIA_1);
	pv = config_get_array_value(arc_config, key);
	strcpy(nombre_nodo_1, pv[0]);
	*bloque_1 = atoi(pv[1]);
	free(key);
	string_iterate_lines(pv, (void *)free);
	free(pv);
}

void archivo_destruir(t_config *arc_config) {
	config_destroy(arc_config);
}
