#ifndef SRC_OP_NODOS_H_
#define SRC_OP_NODOS_H_

#include <stdbool.h>
#include <commons/collections/list.h>
#include "../../common/global.h"
#include "../../common/socket.h"
#include "../../common/protocol.h"
#include "nodo.h"
#include "bitmap.h"

typedef struct {
	socket_t socket;
	char nombre_nodo[NOMBRE_NODO_SIZE];
	int cant_bloques;
	char ip[IP_SIZE];
	char puerto[PUERTO_SIZE];
} nodo_detalle_t;

void nodos_inicializar();
bool nodos_registrar(packet_t *packet, socket_t sockDN, yamafs_t *config, bool *esperarDNs, bool *estadoEstable);
bool nodos_informar(packet_t *packet, socket_t sockYAMA);
nodo_detalle_t *nodos_obtener_datos_nodo(const char *nombre_nodo);
void nodos_destruir();

#endif /* SRC_OP_NODOS_H_ */
