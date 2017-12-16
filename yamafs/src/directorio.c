#include "directorio.h"

#define KEY_PRE "INDEX"
#define KEY_DIR "DIRECTORIO"
#define KEY_PAD "PADRE"
#define ROOT_NAME "root"
#define MAX_DIR 100
#define DIR_NOM_VACIO ""
#define DIR_PAD_VACIO -2

t_config *dir_config;
directorio_t dir[MAX_DIR];

bool directorio_existe_config(yamafs_t *config) {
	char *path = string_from_format("%sdirectorio.dat", config->metadata_path);
	return access(path, F_OK) != -1;
}

void directorio_borrar(yamafs_t *config) {
	char *path = string_from_format("%sdirectorio.dat", config->metadata_path);
	global_delete_file(path);
	free(path);
}

void directorio_crear(yamafs_t *config) {
	char *path = string_from_format("%sdirectorio.dat", config->metadata_path);

	if(!global_create_txtfile(path, NULL, 0)) exit(EXIT_FAILURE);

	dir_config = config_create(path);
	int i;
	char *key;
	for(i = 0; i < MAX_DIR; i++) {
		dir[i].index = i;
		if(i == 0) {
			strcpy(dir[i].nombre, ROOT_NAME);
			dir[i].padre = -1;
		}
		else {
			strcpy(dir[i].nombre, DIR_NOM_VACIO);
			dir[i].padre = DIR_PAD_VACIO;
		}

		key = string_from_format("%s%d%s", KEY_PRE, i, KEY_DIR);
		config_set_value(dir_config, key, dir[i].nombre);
		free(key);

		key = string_from_format("%s%d%s", KEY_PRE, i, KEY_PAD);
		config_set_value(dir_config, key, string_itoa(dir[i].padre));
		free(key);
	}
	config_save(dir_config);

	free(path);
}

void directorio_cargar(yamafs_t *config) {
	char *path = string_from_format("%sdirectorio.dat", config->metadata_path);

	dir_config = config_create(path);

	int i;
	char *key, *val;
	for(i = 0; i < MAX_DIR; i++) {
		dir[i].index = i;

		key = string_from_format("%s%d%s", KEY_PRE, i, KEY_DIR);
		val = config_get_string_value(dir_config, key); //val no se libera porque se modifica el tad del config
		if(val == NULL) config_set_value(dir_config, key, DIR_NOM_VACIO);
		strcpy(dir[i].nombre, val == NULL ? DIR_NOM_VACIO : val);
		free(key);

		key = string_from_format("%s%d%s", KEY_PRE, i, KEY_PAD);
		dir[i].padre = config_get_int_value(dir_config, key);
		free(key);
	}
}

static directorio_t *buscar_por_nombre_y_padre(char *dir_name, int padre) {
	int i;
	for(i = 0; i < MAX_DIR; i++)
		if(string_equals_ignore_case(dir[i].nombre, dir_name) && dir[i].padre == padre)
			return &dir[i];
	return NULL;
}

static bool tiene_subdirectorios(directorio_t *directorio) {
	int i;
	for(i = 0; i < MAX_DIR; i++)
		if(dir[i].padre == directorio->index)
			return true;
	return false;
}

static directorio_t *crear_directorio(char *dir_name, int padre) {
	//buscar nodo libre
	int i;
	for(i = 0; i < MAX_DIR; i++)
		if(dir[i].padre == DIR_PAD_VACIO) break;
	//si no encontro returnar nulo
	if(i == MAX_DIR) return NULL;
	//si encontro modificar nodo
	strcpy(dir[i].nombre, dir_name);
	dir[i].padre = padre;
	//guardar config
	char *key = string_from_format("%s%d%s", KEY_PRE, dir[i].index, KEY_DIR);
	config_set_value(dir_config, key, dir[i].nombre);
	free(key);
	key = string_from_format("%s%d%s", KEY_PRE, dir[i].index, KEY_PAD);
	config_set_value(dir_config, key, string_itoa(dir[i].padre));
	free(key);
	config_save(dir_config);

	return &dir[i];
}

static void borrar_directorio(directorio_t *directorio) {
	//actualizar nodo
	strcpy(directorio->nombre, DIR_NOM_VACIO);
	directorio->padre = DIR_PAD_VACIO;
	//guardar config
	char *key = string_from_format("%s%d%s", KEY_PRE, directorio->index, KEY_DIR);
	config_set_value(dir_config, key, directorio->nombre);
	free(key);
	key = string_from_format("%s%d%s", KEY_PRE, directorio->index, KEY_PAD);
	config_set_value(dir_config, key, string_itoa(directorio->padre));
	free(key);
	config_save(dir_config);
}

int directorio_crear_dir(char *dir_path) {
	if(!string_starts_with(dir_path, "/")) return -1;

	char **dirs = string_split(dir_path, "/");
	directorio_t *directorio = buscar_por_nombre_y_padre(ROOT_NAME, -1);
	int padre = directorio->index;
	bool crearDirectorio = false;
	int rdo = 0;
	void iterar(char *d) {
		if(!crearDirectorio) {
			//buscar nodo
			directorio = buscar_por_nombre_y_padre(d, padre);
			//obtener padre o flag
			if(directorio != NULL) padre = directorio->index;
			else crearDirectorio = true;
		}
		if(crearDirectorio) {
			directorio = crear_directorio(d, padre);
			if(directorio == NULL) {
				rdo = -2;
				return;
			}
			padre = directorio->index;
		}
	}
	string_iterate_lines(dirs, (void*)iterar);

	string_iterate_lines(dirs, (void*)free);
	free(dirs);
	return rdo;
}

int directorio_borrar_dir(char *dir_path) {
	if(!string_starts_with(dir_path, "/")) return -1;

	char **dirs = string_split(dir_path, "/");
	directorio_t *directorio = buscar_por_nombre_y_padre(ROOT_NAME, -1);
	int padre = directorio->index;
	bool existeDirectorio = true;
	int rdo = 0;
	void iterar(char *d) {
		if(existeDirectorio) {
			//buscar nodo
			directorio = buscar_por_nombre_y_padre(d, padre);
			//obtener padre o flag
			if(directorio != NULL) padre = directorio->index;
			else existeDirectorio = false;
		}
	}
	string_iterate_lines(dirs, (void*)iterar);
	if(existeDirectorio) {
		//buscar si directorio tiene subdirectorios
		if(tiene_subdirectorios(directorio)) rdo = -3;
		else borrar_directorio(directorio);
	}
	else rdo = -2;

	string_iterate_lines(dirs, (void*)free);
	free(dirs);
	return rdo;
}

int directorio_obtener_indice(const char *path, char *archivo) {
	if(!string_starts_with(path, "/")) return -1;

	char **dirs = string_split(path, "/");

	int c = 0;
	void iterar_contar(char *d) { c++; }
	string_iterate_lines(dirs, (void *)iterar_contar);

	bool pathConArchivo = false;
	if(string_contains(path, ".")) {
		pathConArchivo = true;
		strcpy(archivo, dirs[c - 1]);
	}

	directorio_t *directorio = buscar_por_nombre_y_padre(ROOT_NAME, -1);
	int padre = directorio->index;
	bool existeDirectorio = true;
	int rdo = 0;
	void iterar(char *d) {
		if(existeDirectorio && (!pathConArchivo || (pathConArchivo && c > 1))) {
			//buscar nodo
			directorio = buscar_por_nombre_y_padre(d, padre);
			//obtener padre o flag
			if(directorio != NULL) padre = directorio->index;
			else existeDirectorio = false;
		}
		c--;
	}
	string_iterate_lines(dirs, (void*)iterar);

	if(!existeDirectorio) return -1;
	return directorio->index;
}

void directorio_destruir() {
	config_destroy(dir_config);
}
