/*
 * consola.h
 *
 *  Created on: 9/9/2017
 *      Author: utnso
 */

#ifndef SRC_CONSOLA_H_
#define SRC_CONSOLA_H_

#include <commons/string.h>
#include <stdio.h>


#define COMANDO_AYUDA "ayuda"
#define COMANDO_SALIR "salir"

#define COMANDO_FORMATEAR "format"


#define COMANDO_ELIMINAR_ARCHIVO "rm"
#define COMANDO_RENOMBRAR_ARCHIVO "rename"
#define COMANDO_MOVER_ARCHIVO "mv"
#define COMANDO_MOSTRAR_ARCHIVO "cat"
#define COMANDO_COPIAR_ARCHIVO_FS_LOCAL "cpfrom"
#define COMANDO_COPIAR_ARCHIVO_LOCAL_FS "cpto"

#define COMANDO_CREAR_DIRECTORIO "mkdir"

#define COMANDO_COPIAR_BLOQUE "cpblock"
#define COMANDO_MD5 "md5"
#define COMANDO_LISTAR "ls"
#define COMANDO_MOSTRAR_INFO_ARCHIVO "info"

void consola_iniciar();
void consola_remueve_salto_de_linea(char* salida, char* texto);
char* consola_comando_preparado(char* comando);
int consolsa_ejecutar_comando(char* comando);
int consola_mostrar_ayuda(char* parametro);

#endif /* SRC_CONSOLA_H_ */
