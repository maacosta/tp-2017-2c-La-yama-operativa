#ifndef SRC_REDUCCION_H_
#define SRC_REDUCCION_H_

#include <stdbool.h>
#include <commons/collections/list.h>
#include "../../common/global.h"
#include "../../common/socket.h"
#include "../../common/protocol.h"
#include "configuration.h"
#include "filesystem.h"
#include "server.h"

typedef struct {
    int numero_job;
    char nombre_nodo[NOMBRE_NODO_SIZE];
    char ip[IP_SIZE];
    char puerto[PUERTO_SIZE];
    char nombre_archivo_temporal[NOMBRE_ARCHIVO_TMP*10];
    char nombre_archivo_reduccion_local[NOMBRE_ARCHIVO_TMP];
} detalle_reduccion_t;

bool reduccion_iniciar(packet_t *packet, socket_t sockMaster, t_list *estados_master, t_list *nodos);

#endif /* SRC_REDUCCION_H_ */
