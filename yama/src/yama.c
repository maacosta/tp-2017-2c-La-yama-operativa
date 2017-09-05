/*
 * yama.c
 *
 *  Created on: 4/9/2017
 *      Author: utnso
 */

#include "yama.h"
#include "../../common/log.h"


int main(int argc, char **argv) {
	log_init("log", "yama");
	log_msg_error("esto es un error");
	return 0;
}
