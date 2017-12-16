#ifndef SRC_APAREO_H_
#define SRC_APAREO_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <commons/collections/list.h>
#include "../../common/protocol.h"

typedef struct {
	char path[NOMBRE_ARCHIVO_TMP];
	FILE *file;
	char line[1024];
	bool cerrar;
} apareo_archivo_t;

bool apareo_realizar(char **lista_nombre_archivos, char *nombre_resultado, char *path_base);

#endif /* SRC_APAREO_H_ */
