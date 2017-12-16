#include "apareo.h"

static void leer_archivo_hasta_el_final(apareo_archivo_t *arc, FILE *file) {
	while(fgets(&arc->line, sizeof(arc->line), arc->file))
		fprintf(file, "%s", &arc->line);
	fclose(arc->file);
	arc->cerrar = true;
	log_msg_info("apareo | Se cierra el archivo [ %s ] que se leyo hasta el final", &arc->path);
}

bool apareo_realizar(char **lista_nombre_archivos, char *nombre_resultado, char *path_base) {
	char *path = string_from_format("%s%s", path_base, nombre_resultado);

	//crear lista y abrir archivos
	t_list *archivos = list_create();
	apareo_archivo_t *arc;
	bool hay_error = false;
	void iterar(char *arc_tmp) {
		if(hay_error) return;
		char *p = string_from_format("%s%s", path_base, arc_tmp);

		arc = malloc(sizeof(apareo_archivo_t));
		strcpy(&arc->path, p);
		arc->cerrar = false;
		arc->file = fopen(p, "r");
		if(arc->file == NULL) {
			log_msg_error("apareo | No se pudo abrir el archivo [ %s ]", p);
			hay_error = true;
			return;
		}
		log_msg_info("apareo | Se abrio el archivo [ %s ]", &arc->path);
		free(p);
		list_add(archivos, arc);
	}
	string_iterate_lines(lista_nombre_archivos, (void *)iterar);

	if(hay_error) return false;

	//abro archivo resultado
	FILE *file = fopen(path, "w");
	if(file == NULL) {
		log_msg_error("apareo | No se pudo crear el archivo [ %s ]", path);
		return false;
	}

	//si solo es un archivo, escribirlo directamente
	if(list_size(archivos) == 1) {
		log_msg_info("apareo | Solo se detecto un archivo para el apareo [ %s ]", &arc->path);
		arc = list_get(archivos, 0);
		leer_archivo_hasta_el_final(arc, file);
		list_destroy_and_destroy_elements(archivos, (void*)free);
		return true;
	}
	log_msg_info("apareo | Se van a aparear [ %d ] archivos", list_size(archivos));

	//obtengo la primer linea de cada
	int i;
	for(i = 0; i < list_size(archivos); i++) {
		arc = list_get(archivos, i);
		if(fgets(&arc->line, sizeof(arc->line), arc->file) == NULL)
			arc->cerrar = true;
	}

	//quitar elemento si el archivo esta vacio
	bool es_archivo_cerrado(apareo_archivo_t *a) {
		return a->cerrar;
	}
	while((arc = list_remove_by_condition(archivos, (void*)es_archivo_cerrado)) != NULL) {
		log_msg_info("apareo | Se cierra el archivo [ %s ]", &arc->path);
		fclose(arc->file);
		free(arc);
	}

	apareo_archivo_t *arc_a, *arc_b;
	do {
		//quedarme con la linea de menor peso de entre todos los archivos
		arc_a = list_get(archivos, 0);
		for(i = 1; i < list_size(archivos); i++) {
			arc_b = list_get(archivos, i);
			if(strcmp(&arc_a->line, &arc_b->line) > 0)
				arc_a = arc_b;
		}
		//escribir a resultado
		fprintf(file, "%s", &arc_a->line);
		if(arc_a->line[strlen(arc_a->line) - 1] != '\n')
			fputc('\n', file);

		//eliminar nodos si llego al final del archivo
		if(fgets(&arc_a->line, sizeof(arc_a->line), arc_a->file) == NULL) {
			fclose(arc_a->file);
			arc_a->cerrar = true;
			arc = list_remove_by_condition(archivos, (void*)es_archivo_cerrado);
			log_msg_info("apareo | Se cierra el archivo [ %s ] en el apareo", &arc->path);
			free(arc);
		}
	} while(list_size(archivos) > 1);

	arc = list_get(archivos, 0);
	fprintf(file, "%s", &arc->line);
	leer_archivo_hasta_el_final(arc, file);
	list_destroy_and_destroy_elements(archivos, (void*)free);

	log_msg_info("apareo | Se realizo el apareo en el archivo [ %s ]", path);

	fclose(file);
	free(path);

	return true;
}

