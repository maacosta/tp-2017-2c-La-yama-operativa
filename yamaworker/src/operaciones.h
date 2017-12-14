#ifndef SRC_OPERACIONES_H_
#define SRC_OPERACIONES_H_

#include <sys/wait.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <commons/collections/list.h>
#include "../../common/global.h"
#include "../../common/socket.h"
#include "../../common/protocol.h"
#include "../../common/memoria.h"
#include "configuration.h"
#include "server.h"

typedef struct {
    char ip[IP_SIZE];
    char puerto[PUERTO_SIZE];
    char nombre_archivo_local[NOMBRE_ARCHIVO_TMP];
} reduccion_worker_t;

bool op_transformar(packet_t *packet, socket_t sockMaster, yamaworker_t* config);
bool op_reduccion(packet_t *packet, socket_t sockMaster, yamaworker_t* config);
bool op_reduccion_global(packet_t *packet, socket_t sockMaster, yamaworker_t* config);
bool op_almacenamiento_final(packet_t *packet, socket_t sockMaster, yamaworker_t* config);

#endif /* SRC_OPERACIONES_H_ */
