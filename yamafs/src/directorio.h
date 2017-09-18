/*
 * directorio.h
 *
 *  Created on: 18/9/2017
 *      Author: utnso
 */

#ifndef SRC_DIRECTORIO_H_
#define SRC_DIRECTORIO_H_


#include <string.h>
#include <stdlib.h>

typedef struct {
	int id;
	char nombre[255];
	int padreId;
	pthread_rwlock_t lock;
} t_directorio;



t_directorio* directorio_crear();
t_directorio* directorio_crear_raiz();
void directorio_set_nombre(t_directorio* directorio, char* nombre);
void directorio_set_padre(t_directorio* directorio, int padreId);
void directorio_eliminar(t_directorio* directorio);
void log_error_ya_existe_directorio(char* directorio, char* directorio_padre_nuevo);
void log_error_directorio_no_existe(char* ruta_directorio);


#endif /* SRC_DIRECTORIO_H_ */
