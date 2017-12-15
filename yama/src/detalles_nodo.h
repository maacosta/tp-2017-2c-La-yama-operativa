#ifndef SRC_DETALLES_NODO_H_
#define SRC_DETALLES_NODO_H_

#include <stdbool.h>
#include <commons/collections/list.h>
#include "../../common/global.h"
#include "../../common/socket.h"
#include "../../common/protocol.h"
#include "log_planificador.h"

void dn_inicializar();
void dn_agregar_nodo(char *nombre_nodo, char *ip, char *puerto);
void dn_incrementar_carga(detalle_nodo_t *nodo);
void dn_reducir_carga(detalle_nodo_t *nodo);
detalle_nodo_t *dn_buscar_por_nodo(char *nombre_nodo);
int dn_obtener_wl_max();
void dn_finalizar();

#endif /* SRC_DETALLES_NODO_H_ */
