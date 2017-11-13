#ifndef SRC_NODO_H_
#define SRC_NODO_H_

#include <stdlib.h>
#include <commons/collections/list.h>
#include "../../common/protocol.h"
#include <commons/config.h>
#include "configuration.h"

typedef struct {
	char nombre_nodo[NOMBRE_NODO_SIZE];
	int cantidad_bloques;
} datos_nodo_registro_t;

bool nodo_existe_config(yamafs_t *config);
void nodo_borrar(yamafs_t *config);
void nodo_crear(yamafs_t *config);
void nodo_cargar(yamafs_t *config);
void nodo_agregar(datos_nodo_registro_t *nodo);
void nodo_quitar(datos_nodo_registro_t *nodo);
void nodo_destruir();

#endif /* SRC_NODO_H_ */
