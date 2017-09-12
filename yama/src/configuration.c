/*
 * configuration.c
 *
 *  Created on: 11/9/2017
 *      Author: ABDUL GANEM
 *      Comentario: No estoy seguro de como se llama al algoritmo desde la configuracion
 */

#include "configuration.h"

static t_config* c;

yama_t *config_leer(const char* path) {
	if(access(path, F_OK) == -1 ) {
		log_msg_error("No existe el archivo de configuracion %s", path);
		exit(EXIT_FAILURE);
	}

	c = config_create((char *)path);
	yama_t *config = malloc(sizeof(yama_t));

	config->yama_ip 	= config_get_string_value(c, "IP");
	config->yama_puerto = config_get_int_value(c, "PUERTO");
	config->yama_retardo_planificacion = config_get_int_value(c, "RETARDO_PLANIFICACION");
	config->yama_algoritmo_balanceo	= config_get_string_value(c, "ALGORITMO_BALANCEO");
	config->log_file 	= config_get_string_value(c, "LOG_FILE");
	config->log_name 	= config_get_string_value(c, "LOG_NAME");


	return config;
}

void config_liberar(yama_t* config) {
	config_destroy(c);
	free(config);
}

