#ifndef SRC_FILESYSTEM_H_
#define SRC_FILESYSTEM_H_

#include <stdbool.h>
#include <commons/collections/list.h>
#include "../../common/protocol.h"
#include "../../common/global.h"
#include "bitmap.h"
#include "op_nodos.h"

#define TAMANIO_BLOQUE 1048576 /*1 mb*/

typedef struct {
	unsigned char stream[TAMANIO_BLOQUE];
	ssize_t size;
} bloque_t;

bool filesystem_cpfrom(const char *path_origen, const char *nom_archivo, int indice, bool es_txt, yamafs_t *config);
bool filesystem_almacenamiento_final(packet_t *packet, socket_t sockWorker, yamafs_t *config);
bool filesystem_cpto(const char *path_destino, const char *nom_archivo, int indice, yamafs_t *config);
bool filesystem_obtener_datos_archivo(packet_t *packet, socket_t sockWorker, yamafs_t *config);

#endif /* SRC_FILESYSTEM_H_ */
