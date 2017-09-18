/*
 * nodo.c
 *
 *  Created on: 18/9/2017
 *      Author: utnso
 */

#include "nodo.h"

t_nodo* nodo_crear() {
	t_nodo* nodo = malloc(sizeof(t_nodo));
	memset(nodo->nombre, 0, 80);
	nodo->socket = 0;
	nodo->cantidad_bloques_totales = 0;
	nodo->cantidad_bloques_libres = 0;
	memset(nodo->ip, 0, 16);
	nodo->puerto = 0;
	pthread_rwlock_init(&nodo->lock, NULL);
	return nodo;
}

void nodo_set_nombre(t_nodo* nodo, char* nombre) {
	strcpy(nodo->nombre, nombre);
}

void nodo_eliminar(t_nodo* nodo) {
	free(nodo->bloques);
	free(nodo);
}

void nodo_set_socket(t_nodo* nodo, int socket) {
	nodo->socket = socket;
}


void nodo_set_cantidad_bloques_totales(t_nodo* nodo, int cant_bloques) {
	nodo->cantidad_bloques_totales = cant_bloques;
}


void nodo_set_cantidad_bloques_libres(t_nodo* nodo, int cant_bloques) {
	nodo->cantidad_bloques_libres = cant_bloques;
}

//Devuelve el número de bloque disponible, si esta en 1-ocupado.  0-disponible
int nodo_bloque_disponible(t_nodo* nodo) {
	int numero_bloque = 0;

	while ((nodo->bloques[numero_bloque] == 1) && (numero_bloque < nodo->cantidad_bloques_totales)) {
		numero_bloque++;
	}

	return numero_bloque;
}

//Devuelve el número de bloque disponible y  marco al bloque como ocupado y redusco la cant de bloques libres.
int nodo_asignar_bloque_disponible(t_nodo* nodo) {

	int bloque_disponible = nodo_bloque_disponible(nodo);

	nodo->bloques[bloque_disponible] = 1;
	nodo->cantidad_bloques_libres--;

	return bloque_disponible;
}


