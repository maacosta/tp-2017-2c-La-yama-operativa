#include "bitmap.h"

bool bitmap_existe_config(yamafs_t *config, const char *nombre_nodo) {
	char *path = string_from_format("%sbitmaps/%s.dat", config->metadata_path, nombre_nodo);
	return access(path, F_OK) != -1;
}

void bitmap_borrar(yamafs_t *config, const char *nombre_nodo) {
	char *path = string_from_format("%sbitmaps/%s.dat", config->metadata_path, nombre_nodo);
	global_delete_file(path);
	free(path);
}

bitmap_t bitmap_abrir(yamafs_t *config, const char *nombre_nodo, off_t count_bits) {
	char *path = string_from_format("%sbitmaps/%s.dat", config->metadata_path, nombre_nodo);
	int fd_bitmap = open(path, O_RDWR);
	if (fd_bitmap == -1) {
		log_msg_error("bitmap | No se pudo abrir el archivo [ %s ] %s", path, strerror(errno));
		exit(EXIT_FAILURE);
	}
	off_t size = count_bits / 8;
	if (size % 8 != 0) size++;

	struct stat sb_bitmap;
	if (fstat(fd_bitmap, &sb_bitmap) < 0) {
		log_msg_error("bitmap | No se pudo recuperar el tamanio del archivo [ %s ] %s", path, strerror(errno));
		close(fd_bitmap);
		exit(EXIT_FAILURE);
	}
	unsigned char *bmap = mmap(NULL, sb_bitmap.st_size, PROT_WRITE | PROT_READ, MAP_SHARED, fd_bitmap, 0);
	if (bmap == MAP_FAILED) {
		log_msg_error("bitmap | No se pudo mapear len [ %d ] %s", sb_bitmap.st_size, strerror(errno));
		exit(EXIT_FAILURE);
	}
	free(path);

	bitmap_t bm;
	bm.bitmap = bmap;
	bm.bitarray = bitarray_create_with_mode(bmap, size, MSB_FIRST);
	bm.fd = fd_bitmap;

	return bm;
}

bitmap_t bitmap_crear(yamafs_t *config, const char *nombre_nodo, off_t count_bits) {
	char *path_dir = string_from_format("%sbitmaps/", config->metadata_path);
	char *path = string_from_format("%s%s.dat", path_dir, nombre_nodo);
	if(!global_get_dir_exist(path_dir))
		global_create_dir(path_dir);
	if(global_get_file_exist(path))
		global_delete_file(path);

	off_t size = count_bits / 8;
	if (size == 0) size = 1;
	else if (size % 8 != 0) size++;

	char *buffer = malloc(size);
	memset(buffer, 0, size);
	global_create_txtfile(path, buffer, size);
	free(buffer);

	free(path_dir);
	return bitmap_abrir(config, nombre_nodo, count_bits);
}

void bitmap_set(bitmap_t *bm, off_t bit_index) {
	bitarray_set_bit(bm->bitarray, bit_index);
}

bool bitmap_get(bitmap_t *bm, off_t bit_index) {
	return bitarray_test_bit(bm->bitarray, bit_index);
}

void bitmap_clean(bitmap_t *bm, off_t bit_index) {
	bitarray_clean_bit(bm->bitarray, bit_index);
}

void bitmap_destruir(bitmap_t *bm) {
	bitarray_destroy(bm->bitarray);
	munmap(bm->bitmap, bm->bitarray->size);
	close(bm->fd);
}

