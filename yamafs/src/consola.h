#ifndef SRC_CONSOLA_H_
#define SRC_CONSOLA_H_

#include <commons/string.h>
#include <stdio.h>
#include "configuration.h"

#define CMD_AYUDA "ayuda"
#define CMD_SALIR "salir"
#define CMD_FORMATEAR "format"
#define CMD_ELIMINAR_ARCHIVO "rm"
#define CMD_RENOMBRAR_ARCHIVO "rename"
#define CMD_MOVER_ARCHIVO "mv"
#define CMD_MOSTRAR_ARCHIVO "cat"
#define CMD_COPIAR_ARCHIVO_FS_LOCAL "cpfrom"
#define CMD_COPIAR_ARCHIVO_LOCAL_FS "cpto"
#define CMD_CREAR_DIRECTORIO "mkdir"
#define CMD_COPIAR_BLOQUE "cpblock"
#define CMD_MD5 "md5"
#define CMD_LISTAR "ls"
#define CMD_MOSTRAR_INFO_ARCHIVO "info"

void consola_crear(yamafs_t *config);

#endif /* SRC_CONSOLA_H_ */
