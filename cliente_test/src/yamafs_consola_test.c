#include "yamafs_consola_test.h"

void yamafs_consola_leer_escribir_archivo_binario() {
	char *path = "./penguin_linux.jpeg";
	char *path_copy = "./copy_penguin_linux.jpeg";

	ssize_t size;
	unsigned char *buffer = global_read_binfile(path, &size);

	if(global_get_file_exist(path_copy))
		global_delete_file(path_copy);
	global_create_binfile(path_copy, buffer, size);
}

void yamafs_consola_leer_escribir_archivo_texto() {
	char *path = "./texto_prueba.txt";
	char *path_copy = "./copy_texto_prueba.txt";

	ssize_t size;
	unsigned char *buffer = global_read_txtfile(path, &size);

	if(global_get_file_exist(path_copy))
		global_delete_file(path_copy);
	global_create_txtfile(path_copy, buffer, size);
}
