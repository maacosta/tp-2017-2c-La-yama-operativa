#ifndef SRC_DIRECTORIO_H_
#define SRC_DIRECTORIO_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>
#include "configuration.h"

typedef struct {
	int index;
	char nombre[255];
	int padre;
} directorio_t;

bool directorio_existe_config(yamafs_t *config);
void directorio_borrar(yamafs_t *config);
void directorio_crear(yamafs_t *config);
void directorio_cargar(yamafs_t *config);
int directorio_crear_dir(char *dir_path);
int directorio_borrar_dir(char *dir_path);
void directorio_destruir();

#endif /* SRC_DIRECTORIO_H_ */
