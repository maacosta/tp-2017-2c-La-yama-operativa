#include "archivo.h"

t_archivo* archivo_crear() {
	t_archivo* archivo = malloc(sizeof(t_archivo));
	memset(archivo->nombre, 0, 80);
	archivo->padreId = 1;
	archivo->tamanio = 0;
	archivo->cantidad_bloques = 0;
	archivo->disponible = false;
	pthread_rwlock_init(&archivo->lock, NULL);
	return archivo;
}

void archivo_set_nombre(t_archivo* archivo, char* nombre) {
	strcpy(archivo->nombre, nombre);
}

void archivo_set_padre(t_archivo* archivo, int padreId) {
	archivo->padreId = padreId;
}

void archivo_set_tamanio(t_archivo* archivo, int tamanio) {
	archivo->tamanio = tamanio;
}

void archivo_asignar_estado(t_archivo* archivo, bool estado) {
	archivo->disponible = estado;
}
