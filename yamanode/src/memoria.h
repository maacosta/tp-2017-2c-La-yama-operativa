#ifndef SRC_MEMORIA_H_
#define SRC_MEMORIA_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include "configuration.h"

void memoria_abrir(yamaDataNodo_t *config);
int memoria_obtener_tamanio();
void *memoria_obtener_bloque(int offset, int len);
void memoria_almacenar_bloque(int offset, int len, void *bloque);
void memoria_destruir();

#endif /* SRC_MEMORIA_H_ */
