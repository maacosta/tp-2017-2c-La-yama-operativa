#ifndef SRC_SERVER_H_
#define SRC_SERVER_H_

#include <stdbool.h>
#include <commons/collections/list.h>
#include "../../common/global.h"
#include "../../common/socket.h"
#include "../../common/protocol.h"
#include "configuration.h"
#include "operaciones.h"
#include "operaciones_local.h"

void guardar_archivo_tmp(const char *path, const char *nombre_archivo, unsigned char *stream, ssize_t len, bool es_txt);

socket_t conectar_con(char *ip, char *puerto, const char *nombre);

void server_crear(yamaworker_t *config);

void server_liberar();

#endif /* SRC_SERVER_H_ */
