#include "global.h"

bool global_get_file_exist(const char* path) {
	if(access(path, F_OK) == -1 ) {
		log_msg_error("No existe el archivo de configuracion %s", path);
		return false;
	}
	return true;
}
