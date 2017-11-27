#ifndef SRC_NODO_H_
#define SRC_NODO_H_

#include <stdlib.h>
#include <commons/collections/list.h>
#include "../../common/protocol.h"
#include <commons/config.h>
#include "configuration.h"

bool nodo_existe_config(yamafs_t *config);
void nodo_borrar(yamafs_t *config);
void nodo_crear(yamafs_t *config);
void nodo_cargar(yamafs_t *config);
bool nodo_existe(const char *nombre_nodo);
void nodo_notificar_existencia(const char *nombre_nodo);
bool nodo_se_notificaron_todos_los_registrados();
void nodo_agregar(const char *nombre_nodo, int cant_bloques_totales, int cant_bloques_libres);
void nodo_actualizar(const char *nombre_nodo, int cant_bloques_libres);
void nodo_quitar(const char *nombre_nodo);
int nodo_cantidad();
char **nodo_lista_nombre();
void nodo_destruir();

#endif /* SRC_NODO_H_ */
