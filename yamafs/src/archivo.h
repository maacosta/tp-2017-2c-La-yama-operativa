#ifndef SRC_ARCHIVO_H_
#define SRC_ARCHIVO_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>
#include "configuration.h"

bool archivo_existe_config_nombre(yamafs_t *config, const char *nombre_archivo, int indice);
void archivo_borrar(yamafs_t *config, const char *nombre_archivo, int indice);
t_config *archivo_cargar(yamafs_t *config, const char *nombre_archivo, int indice);
void archivo_definir_cabecera(t_config *arc_config, int cant_bloques, int tamanio, bool es_txt);
void archivo_recuperar_cabecera(t_config *arc_config, int *cant_bloques, int *tamanio, bool *es_txt);
void archivo_definir_copias(t_config *arc_config, int indice, int bytes, char *nombre_nodo_0, int bloque_0, char *nombre_nodo_1, int bloque_1);
void archivo_recuperar_copias(t_config *arc_config, int indice, int *bytes, char *nombre_nodo_0, int *bloque_0, char *nombre_nodo_1, int *bloque_1);
void archivo_destruir(t_config *arc_config);

#endif /* SRC_ARCHIVO_H_ */
