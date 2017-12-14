#ifndef SRC_OP_REDUCCION_H_
#define SRC_OP_REDUCCION_H_

#include "main.h"
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

void ejecutar_reduccion(socket_t sockYama, bool es_txt_reductor, char *archivo_reductor);

void ejecutar_reduccion_global(socket_t sockYama, bool es_txt_reductor, char *archivo_reductor);

#endif /* SRC_OP_REDUCCION_H_ */
