#ifndef SRC_MEMORIA_H_
#define SRC_MEMORIA_H_

#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <stdlib.h>

void memoria_abrir(const char *path_databin, bool es_solo_lectura);
int memoria_obtener_tamanio();
unsigned char *memoria_obtener_bloque(int offset, int len);
void memoria_almacenar_bloque(int offset, int len, unsigned char *bloque);
void memoria_destruir();

#endif /* SRC_MEMORIA_H_ */
