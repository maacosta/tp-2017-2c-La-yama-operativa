#include "directorio.h"

t_config *dir_config;
directorio_t dir[100];

bool directorio_existe_config(yamafs_t *config) {
	char *path = string_from_format("./%s/directorio.dat", config->metadata_path);
	return access(path, F_OK) != -1;
}

void directorio_borrar(yamafs_t *config) {
	char *path = string_from_format("./%s/directorio.dat", config->metadata_path);
	if(access(path, F_OK) != -1) {
		if(remove(path) != 0) {
			log_msg_error("No se pudo borrar el archivo %s", path);
			exit(EXIT_FAILURE);
		}
	}
	free(path);
}

static void crear_archivo(char *filepath) {
	FILE *d = fopen(filepath, "w");
	if(d == NULL) {
		log_msg_error("No se pudo crear el archivo %s", filepath);
		exit(EXIT_FAILURE);
	}
	fclose(d);
}

void directorio_crear(yamafs_t *config) {
	char *path = string_from_format("./%s/directorio.dat", config->metadata_path);

	crear_archivo(path);

	dir_config = config_create(path);
	int i;
	char *key;
	for(i = 0; i < 100; i++) {
		dir[i].index = i;
		if(i == 0) {
			strcpy(dir[i].nombre, "root");
			dir[i].padre = -1;
		}
		else {
			strcpy(dir[i].nombre, "");
			dir[i].padre = -2;
		}

		key = string_from_format("%s%d%s", "INDEX", i, "DIRECTORIO");
		config_set_value(dir_config, key, dir[i].nombre);
		free(key);

		key = string_from_format("%s%d%s", "INDEX", i, "PADRE");
		config_set_value(dir_config, key, string_itoa(dir[i].padre));
		free(key);
	}
	config_save(dir_config);

	free(path);
}

static directorio_t *buscar_por_nombre_y_padre(char *dir_name, int padre) {
	int i;
	for(i = 0; i < 100; i++)
		if(string_equals_ignore_case(dir[i].nombre, dir_name) && dir[i].padre == padre)
			return &dir[i];
	return NULL;
}

static directorio_t *crear_directorio(char *dir_name, int padre) {
	//buscar nodo libre
	int i;
	for(i = 0; i < 100; i++)
		if(dir[i].padre == -2) break;
	//si no encontro returnar nulo
	if(i == 100) return NULL;
	//si encontro modificar nodo
	strcpy(dir[i].nombre, dir_name);
	dir[i].padre = padre;
	//guardar config
	char *key = string_from_format("%s%d%s", "INDEX", dir[i].index, "DIRECTORIO");
	config_set_value(dir_config, key, dir[i].nombre);
	free(key);
	key = string_from_format("%s%d%s", "INDEX", dir[i].index, "PADRE");
	config_set_value(dir_config, key, string_itoa(dir[i].padre));
	free(key);
	config_save(dir_config);

	return &dir[i];
}

int directorio_crear_dir(char *dir_path) {
	if(!string_starts_with(dir_path, "/")) return -1;

	char **dirs = string_split(dir_path, "/");
	directorio_t *directorio = buscar_por_nombre_y_padre("root", -1);
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

void directorio_destruir() {
	config_destroy(dir_config);
}
