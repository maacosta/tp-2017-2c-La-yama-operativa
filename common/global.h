#ifndef global_h
#define global_h

#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include "log.h"

typedef enum {
	YAMA = 1,
	FS = 2,
	MASTER = 3,
	DATANODE = 4,
	WORKER = 5
} process_t;

bool global_get_file_exist(const char* path);

void global_set_process(process_t process);

process_t global_get_process();

ssize_t global_readfile(const char *path, char *buffer);

#endif /* global_h */
