#ifndef SRC_OP_TRANSFORMACION_H_
#define SRC_OP_TRANSFORMACION_H_

#include <commons/collections/list.h>
#include "../../common/thread.h"
#include "../../common/protocol.h"
#include "../../common/socket.h"
#include "main.h"

typedef struct {
	socket_t sockYama;
	unsigned char *payload;
	char *nombre_archivo_transformador;
	bool es_txt_archivo_transformador;
} transformacion_hilo_data_t;

void ejecutar_transformacion(socket_t sockYama, bool es_txt_transformador, char *archivo_transformador, char *archivo_origen, int *num_job);

#endif /* SRC_OP_TRANSFORMACION_H_ */
