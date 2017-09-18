/*
 * directorio.c
 *
 *  Created on: 18/9/2017
 *      Author: utnso
 */
#include "directorio.h"


t_directorio* directorio_crear() {
	t_directorio* directorio = malloc(sizeof(t_directorio));
	memset(directorio->nombre, 0, 255);
	directorio->padreId = 1;
	pthread_rwlock_init(&directorio->lock, NULL);
	return directorio;
}

t_directorio* directorio_crear_raiz() {
	t_directorio* raiz = directorio_crear();
	directorio_set_nombre(raiz, "/");
	directorio_set_padre(raiz, 0);
	raiz->id = 1;
	return raiz;
}

void directorio_set_nombre(t_directorio* directorio, char* nombre) {
	strcpy(directorio->nombre, nombre);
}

void directorio_set_padre(t_directorio* directorio, int padreId) {
	directorio->padreId = padreId;
}

void directorio_eliminar(t_directorio* directorio) {
	free(directorio);
}


void log_error_ya_existe_directorio(char* directorio, char* directorio_padre_nuevo) {
	//logear el error que ya existe el directorio a en b

}

void log_error_directorio_no_existe(char* ruta_directorio) {
	//logear el error de que el directorio a no existe en la ruta de directorio

}

