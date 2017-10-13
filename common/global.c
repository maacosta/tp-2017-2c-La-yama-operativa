#include "global.h"

static process_t g_process;

bool global_get_file_exist(const char* path) {
	if(access(path, F_OK) == -1 ) {
		log_msg_error("No existe el archivo de configuracion %s", path);
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

ssize_t global_readfile(const char *path, char *buffer) {
	FILE *file = fopen(path, "r");
	if(file == NULL) {
		log_msg_error("No se pudo abrir el archivo %s", path);
		return -1;
	}

	fseek(file, 0, SEEK_END);
	long fsize = ftell(file);
	fseek(file, 0, SEEK_SET);
	buffer = (char *)malloc(fsize * sizeof(char));
	fread(buffer, fsize, 1, file);

	bool ok = ferror(file) == 0;
	fclose(file);
	buffer[fsize] = '\0';
	return ok ? fsize : -1;
}
