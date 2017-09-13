/*
 * archivo.h
 *
 *  Created on: 13/9/2017
 *      Author: utnso
 */

#ifndef SRC_ARCHIVO_H_
#define SRC_ARCHIVO_H_

#include <stdbool.h>
#include <stdlib.h>


typedef struct {
	char nombre_nodo[80];
	int bloque_nodo;
	int tamanio_bloque;
	bool conectado;
} t_copia;

typedef struct {
	int cantidad_copias;
	t_copia* copias;
} t_bloque;


typedef struct {
	int id;
	char nombre[255];
	long tamanio;
	int padreId;
	int cantidad_bloques;
	int cantidad_copias_totales;
	t_bloque* bloques;
	bool disponible;
	pthread_rwlock_t lock;
} t_archivo;


#endif /* SRC_ARCHIVO_H_ */
