#ifndef SRC_REDUCCION_H_
#define SRC_REDUCCION_H_

#include "../../common/thread.h"
#include "../../common/protocol.h"
#include "../../common/socket.h"

typedef struct {
    char nombre_nodo[NOMBRE_NODO_SIZE];
    char ip[IP_SIZE];
    char puerto[PUERTO_SIZE];
    char nombre_archivo_local[NOMBRE_ARCHIVO_TMP];
    char nombre_archivo_global[NOMBRE_ARCHIVO_TMP];
    bool encargado;
} reduccion_global_t;

void ejecutar_reduccion(socket_t sockYama, char *archivo_reductor);

void ejecutar_reduccion_global(socket_t sockYama, char *archivo_reductor);

#endif /* SRC_REDUCCION_H_ */
