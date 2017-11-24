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

unsigned char *global_read_txtfile(const char *path, ssize_t *size);
unsigned char *global_read_binfile(const char *path, ssize_t *size);

bool global_create_txtfile(const char *path, unsigned char *buffer, ssize_t len);
bool global_create_binfile(const char *path, unsigned char *buffer, ssize_t len);

void global_delete_file(const char *filepath);

#endif /* global_h */
