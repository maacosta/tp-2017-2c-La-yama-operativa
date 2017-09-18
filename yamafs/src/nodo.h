/*
 * nodo.h
 *
 *  Created on: 18/9/2017
 *      Author: utnso
 */

#ifndef SRC_NODO_H_
#define SRC_NODO_H_

#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <commons/collections/list.h>
#include <sys/socket.h>




typedef struct {
	char nombre[80];
	int socket;
	int cantidad_bloques_totales;
	int cantidad_bloques_libres;
	int* bloques;
	char ip[16];
	int puerto;
	pthread_rwlock_t lock;
} t_nodo;


t_nodo* nodo_crear();
void nodo_set_nombre(t_nodo* nodo, char* nombre);
void nodo_eliminar(t_nodo* nodo);
void nodo_set_socket(t_nodo* nodo, int socket);
void nodo_set_cantidad_bloques_totales(t_nodo* nodo, int cant_bloques);
void nodo_set_cantidad_bloques_libres(t_nodo* nodo, int cant_bloques);
int nodo_bloque_disponible(t_nodo* nodo);
int nodo_asignar_bloque_disponible(t_nodo* nodo);





#endif /* SRC_NODO_H_ */
