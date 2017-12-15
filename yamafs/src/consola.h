#ifndef SRC_CONSOLA_H_
#define SRC_CONSOLA_H_

#include <commons/string.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/signalfd.h>
#include <signal.h>
#include "main.h"
#include "configuration.h"
#include "filesystem.h"

void consola_iniciar(yamafs_t* config, pthread_t th_srv);

#endif /* SRC_CONSOLA_H_ */
