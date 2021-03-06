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
	srand(time(NULL));   // should only be called once
}

process_t global_get_process() {
	return g_process;
}

static unsigned char *read_file(const char *path, ssize_t *size, bool is_txt) {
	FILE *file = fopen(path, is_txt ? "r" : "rb");
	if(file == NULL) {
		log_msg_error("global | No se pudo abrir el archivo [ %s ]", path);
		return NULL;
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
	log_msg_info("global | Se escribe archivo TXT [ %s ] len [ %d ]", path, len);
	return create_file(path, buffer, len, true);
}

bool global_create_binfile(const char *path, unsigned char *buffer, ssize_t len) {
	log_msg_info("global | Se escribe archivo BIN [ %s ] len [ %d ]", path, len);
	return create_file(path, buffer, len, false);
}

void global_delete_file(const char *filepath) {
	if(access(filepath, F_OK) != -1) {
		if(remove(filepath) != 0) {
			log_msg_error("global | No se pudo borrar el archivo [ %s ]", filepath);
		}
	}
}

int global_delete_directory(const char *path) {
	DIR *d = opendir(path);
	size_t path_len = strlen(path);
	int r = -1;

	if(d) {
		struct dirent *p;
		r = 0;
		while (!r && (p=readdir(d))) {
			int r2 = -1;
			char *buf;
			size_t len;

			/* Skip the names "." and ".." as we don't want to recurse on them. */
			if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
				continue;

			len = path_len + strlen(p->d_name) + 2;
			buf = malloc(len);

			if (buf) {
				struct stat statbuf;
				snprintf(buf, len, "%s/%s", path, p->d_name);

				if (!stat(buf, &statbuf)) {
					if (S_ISDIR(statbuf.st_mode))
						r2 = global_delete_directory(buf);
					else
						r2 = unlink(buf);
				}

				free(buf);
			}
			r = r2;
		}
		closedir(d);
	}

	if (!r) {
		r = rmdir(path);
	}

	log_msg_info("global | Se borro el directorio [ %s ] y todo su contenido", path);
	return r;
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

int global_rnd(int desde, int hasta) {
	float f = (float)rand() / (float)RAND_MAX;
	return desde + f * hasta;      // returns a pseudo-random integer between 0 and RAND_MAX
}

void global_nombre_aleatorio(char *prefix, char *dest, size_t length) {
    char charset[] = "0123456789"
                     "abcdefghijklmnopqrstuvwxyz"
                     "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    int i, len = string_length(prefix);
    for (i = 0; i < len; i++)
    	*dest++ = prefix[i];
    while (length-- > 0) {
        size_t index = (double) rand() / RAND_MAX * (sizeof charset - 1);
        *dest++ = charset[index];
    }
    *dest = '\0';
}
