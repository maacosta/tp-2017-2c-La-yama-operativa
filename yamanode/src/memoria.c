#include "memoria.h"

int fd_databin;
struct stat sb_databin;

void memoria_abrir(yamaDataNodo_t *config) {
	fd_databin = open(config->path_databin, O_RDWR);
	if (fd_databin == -1) {
		log_msg_error("No se pudo abrir el archivo [ %s ] %s", config->path_databin, strerror(errno));
		exit(EXIT_FAILURE);
	}
	// To obtain file size
	if (fstat(fd_databin, &sb_databin) == -1){
		log_msg_error("No se pudo recuperar el tamanio del archivo [ %s ] %s", config->path_databin, strerror(errno));
		exit(EXIT_FAILURE);
	}
}

int memoria_obtener_tamanio() {
	return sb_databin.st_size;
}

void *memoria_obtener_bloque(int offset, int len) {
	long int offsetmmap = offset * 1024 * 1024 * sizeof(char);
    void *addr = mmap(NULL, len, PROT_READ, MAP_SHARED, fd_databin, offsetmmap);
    if (addr == MAP_FAILED) {
    	log_msg_error("No se pudo mapear offset [ %d ] len [ %d ] %s", offset, len, strerror(errno));
    	exit(EXIT_FAILURE);
    }
    void *bloque = malloc(len);
    memcpy(bloque, addr, len);
    munmap(addr, len);
    return bloque;
}

void memoria_almacenar_bloque(int offset, int len, void *bloque) {
	long int offsetmmap = offset * 1024 * 1024 * sizeof(char);
    void *addr = mmap(NULL, len, PROT_WRITE, MAP_SHARED, fd_databin, offsetmmap);
    if (addr == MAP_FAILED) {
    	log_msg_error("No se pudo mapear offset [ %d ] len [ %d ] %s", offset, len, strerror(errno));
    	exit(EXIT_FAILURE);
    }
    memcpy(addr, bloque, len);
    munmap(addr, len);
}

void memoria_destruir() {
	close(fd_databin);
}
