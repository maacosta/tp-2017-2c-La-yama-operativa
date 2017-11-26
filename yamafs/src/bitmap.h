#ifndef SRC_BITMAP_H_
#define SRC_BITMAP_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>
#include <commons/bitarray.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include "configuration.h"
#include "../../common/global.h"

typedef struct {
	unsigned char *bitmap;
	t_bitarray *bitarray;
	int fd;
} bitmap_t;

bool bitmap_existe_config(yamafs_t *config, const char *nombre_nodo);
void bitmap_borrar(yamafs_t *config, const char *nombre_nodo);
bitmap_t bitmap_abrir(yamafs_t *config, const char *nombre_nodo, off_t count_bits);
bitmap_t bitmap_crear(yamafs_t *config, const char *nombre_nodo, off_t count_bits);
void bitmap_set(bitmap_t *bm, off_t bit_index);
bool bitmap_get(bitmap_t *bm, off_t bit_index);
void bitmap_clean(bitmap_t *bm, off_t bit_index);
void bitmap_destruir();

#endif /* SRC_BITMAP_H_ */
