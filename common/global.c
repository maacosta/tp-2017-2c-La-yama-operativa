#include "global.h"

static process_t g_process;

bool global_get_file_exist(const char* path) {
	if(access(path, F_OK) == -1 ) {
		log_msg_error("global | No existe el archivo [ %s ]", path);
		return false;
	}
	return true;
}

void global_set_process(process_t process) {
	g_process = process;
}

process_t global_get_process() {
	return g_process;
}

static unsigned char *read_file(const char *path, ssize_t *size, bool is_txt) {
	FILE *file = fopen(path, is_txt ? "r" : "rb");
	if(file == NULL) {
		log_msg_error("global | No se pudo abrir el archivo [ %s ]", path);
		return -1;
	}

	fseek(file, 0, SEEK_END);
	long fsize = ftell(file);
	fseek(file, 0, SEEK_SET);
	unsigned char *buffer = malloc(fsize * sizeof(unsigned char));
	fread(buffer, fsize, 1, file);

	bool ok = ferror(file) == 0;
	fclose(file);

	*size = ok ? fsize : -1;
	return buffer;
}

unsigned char *global_read_txtfile(const char *path, ssize_t *size) {
	return read_file(path, size, true);
}

unsigned char *global_read_binfile(const char *path, ssize_t *size) {
	return read_file(path, size, false);
}

static bool create_file(const char *path, unsigned char *buffer, ssize_t len, bool is_txt) {
	FILE *file = fopen(path, is_txt ? "w" : "wb");
	if(file == NULL) {
		log_msg_error("global | No se pudo crear el archivo [ %s ]", path);
		return false;
	}

	if(len > 0)
		if(fwrite(buffer, 1, len, file) != len) {
			fclose(file);
			log_msg_error("global | No se pudo escribir en el archivo [ %s ]", path);
			return false;
		}

	fclose(file);
	return true;
}

bool global_create_txtfile(const char *path, unsigned char *buffer, ssize_t len) {
	return create_file(path, buffer, len, true);
}

bool global_create_binfile(const char *path, unsigned char *buffer, ssize_t len) {
	return create_file(path, buffer, len, false);
}

void global_delete_file(const char *filepath) {
	if(access(filepath, F_OK) != -1) {
		if(remove(filepath) != 0) {
			log_msg_error("global | No se pudo borrar el archivo [ %s ]", filepath);
		}
	}
}

bool global_get_dir_exist(const char *path) {
	DIR* dir = opendir(path);
	bool r = false;
	if (dir) {
	    /* Directory exists. */
	    closedir(dir);
	    r = true;
	}
	else if (ENOENT == errno) {
	    /* Directory does not exist. */
		return false;
	}
	else {
		log_msg_error("global | Error al leer directorio [ %s ] %s", path, strerror(errno));
	}
	return r;
}

void global_create_dir(const char *path) {
	mkdir(path, 0700);
}
