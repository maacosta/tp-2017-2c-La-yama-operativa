#include "memoria.h"

int fd_databin;
struct stat sb_databin;

void memoria_abrir(yamaDataNodo_t *config) {
	fd_databin = open(config->path_databin, O_RDWR);
	if (fd_databin == -1) {
		log_msg_error("memoria | No se pudo abrir el archivo [ %s ] %s", config->path_databin, strerror(errno));
		exit(EXIT_FAILURE);
	}
	// To obtain file size
	if (fstat(fd_databin, &sb_databin) == -1){
		log_msg_error("memoria | No se pudo recuperar el tamanio del archivo [ %s ] %s", config->path_databin, strerror(errno));
		close(fd_databin);
		exit(EXIT_FAILURE);
	}
}

int memoria_obtener_tamanio() {
	return sb_databin.st_size;
}

unsigned char *memoria_obtener_bloque(int offset, int len) {
	long int offsetmmap = offset * 1024 * 1024 * sizeof(unsigned char);
	unsigned char *addr = mmap(NULL, len, PROT_READ, MAP_SHARED, fd_databin, offsetmmap);
    if (addr == MAP_FAILED) {
    	log_msg_error("memoria | No se pudo mapear offset [ %d ] len [ %d ] %s", offset, len, strerror(errno));
    	exit(EXIT_FAILURE);
    }
    unsigned char *bloque = malloc(len * sizeof(unsigned char));
    memcpy(bloque, addr, len);
    munmap(addr, len);
    log_msg_info("memoria | Se lee offset [ %d ] len [ %d ]", offset, len);
    return bloque;
}

void memoria_almacenar_bloque(int offset, int len, unsigned char *bloque) {
	long int offsetmmap = offset * 1024 * 1024 * sizeof(unsigned char);
	unsigned char *addr = mmap(NULL, len, PROT_WRITE, MAP_SHARED, fd_databin, offsetmmap);
    if (addr == MAP_FAILED) {
    	log_msg_error("memoria | No se pudo mapear offset [ %d ] len [ %d ] %s", offset, len, strerror(errno));
    	exit(EXIT_FAILURE);
    }
    memcpy(addr, bloque, len);
    munmap(addr, len);
    log_msg_info("memoria | Se almacena offset [ %d ] len [ %d ]", offset, len);
}

void memoria_destruir() {
	close(fd_databin);
}
