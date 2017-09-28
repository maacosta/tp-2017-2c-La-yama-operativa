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
