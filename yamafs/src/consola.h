#ifndef SRC_CONSOLA_H_
#define SRC_CONSOLA_H_

#include <commons/string.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "configuration.h"
#include "md5.h"
#include "filesystem.h"

void consola_iniciar(yamafs_t* config);

#endif /* SRC_CONSOLA_H_ */
