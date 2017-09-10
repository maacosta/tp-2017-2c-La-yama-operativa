/*
 * consola.c
 *
 *  Created on: 9/9/2017
 *      Author: utnso
 */
#include "consola.h"

void iniciar_consola() {

	//En realidad no importa el tamaño del buffer porque si se pasa getLine se da cuenta y hace un realloc :D
	size_t buffer_size = 100;
	char* comando = (char *) calloc(1, buffer_size);
	puts("Ingrese un comando, ayuda o salir");
	while (!string_equals_ignore_case(comando, "salir\n")) {
		printf(">");
		int bytes_read = getline(&comando, &buffer_size, stdin);

		if (bytes_read == -1) {
			//log_error_consola("Error en getline");
		}
		if (bytes_read == 1) {
			continue;
		}
		if (ejecutar_comando(comando) == 0) {
			char* comando_listo = comando_preparado(comando);
			//log_debug_interno("El comando %s fue ejecutado con exito", comando_listo);
		}
	}
	free(comando);
}

int ejecutar_comando(char* comando) {
	int ret;
	char* comando_listo = comando_preparado(comando);
	char** parametros = string_n_split(comando_listo, 6, " ");
	if (string_equals_ignore_case(parametros[0], COMANDO_AYUDA)) {
		ret = mostrar_ayuda(parametros[1]);
		return ret;
	}else if (string_equals_ignore_case(parametros[0], COMANDO_FORMATEAR)) {

		return ret;
	}else if (string_equals_ignore_case(parametros[0], COMANDO_ELIMINAR_ARCHIVO)) {
		if (parametros[1] == NULL) {
					//logearErrorParametrosFaltantes
					return 1;
		}

		return ret;
	}else if (string_equals_ignore_case(parametros[0], COMANDO_RENOMBRAR_ARCHIVO)) {
		if (parametros[1] == NULL || parametros[2] == NULL) {
			//logearErrorParametrosFaltantes
			return 1;
		}

		return ret;
	}else if (string_equals_ignore_case(parametros[0], COMANDO_MOVER_ARCHIVO)) {
		if (parametros[1] == NULL || parametros[2] == NULL) {
			//logearErrorParametrosFaltantes
			return 1;
		}

		return ret;
	}else if (string_equals_ignore_case(parametros[0], COMANDO_MOSTRAR_ARCHIVO)) {
		if (parametros[1] == NULL) {
					//logearErrorParametrosFaltantes
					return 1;
		}

		return ret;
	}else if (string_equals_ignore_case(parametros[0], COMANDO_COPIAR_ARCHIVO_FS_LOCAL)) {
		if (parametros[1] == NULL || parametros[2] == NULL) {
			//logearErrorParametrosFaltantes
			return 1;
		}

		return ret;
	}else if (string_equals_ignore_case(parametros[0], COMANDO_COPIAR_ARCHIVO_LOCAL_FS)) {
		if (parametros[1] == NULL || parametros[2] == NULL) {
			//logearErrorParametrosFaltantes
			return 1;
		}

		return ret;
	}else if (string_equals_ignore_case(parametros[0], COMANDO_CREAR_DIRECTORIO)) {
		if (parametros[1] == NULL) {
			//logearErrorParametrosFaltantes
			return 1;
		}

		return ret;
	}else if (string_equals_ignore_case(parametros[0], COMANDO_COPIAR_BLOQUE)) {
		if (parametros[1] == NULL || parametros[2] == NULL || parametros[3] == NULL) {
			//logearErrorParametrosFaltantes
			return 1;
		}

		return ret;
	}else if (string_equals_ignore_case(parametros[0], COMANDO_MD5)) {
		if (parametros[1] == NULL) {
			//logearErrorParametrosFaltantes
			return 1;
		}

		return ret;
	}else if (string_equals_ignore_case(parametros[0], COMANDO_LISTAR)) {
		if (parametros[1] == NULL) {
			//logearErrorParametrosFaltantes
			return 1;
		}

		return ret;
	}else if (string_equals_ignore_case(parametros[0], COMANDO_MOSTRAR_INFO_ARCHIVO)) {
		if (parametros[1] == NULL) {
			//logearErrorParametrosFaltantes
			return 1;
		}

		return ret;
	}else {
		//logError
		return 1;
	}
	return 0;

}

void remueve_salto_de_linea(char* salida, char* texto) {
	strncpy(salida, texto, strlen(texto) - 1);
}

char* comando_preparado(char* comando) {
	char* comando_listo = calloc(1, strlen(comando));
	remueve_salto_de_linea(comando_listo, comando);
	return comando_listo;
}

int mostrar_ayuda(char* parametro) {
	if (parametro == NULL) {
		puts(
				"Accion 			=> Comando\n---------------------	=> -----------------\nFORMATER 		=> format\nELIMINAR ARCHIVO, DIRECTORIO, NODO O BLOQUE	=> rm\nRENOMBRAR ARCHIVO O DIRECTORIO	=> rename\nMOVER ARCHIVO O DIRECTORIO	=> mv\nMOSTRAR ARCHIVO 	=> cat\nCREAR DIRECTORIO 	=> mkdir\nCOPIAR ARCHIVO LOCAL A FS 	=> cpfrom\nCOPIAR ARCHIVO DE FS A LOCAL 	=> cpto\nCOPIAR BLOQUE 	=> cpblock\nMD5	=> md5\nLISTAR ARCHIVOS 			=> ls\nMUESTRA INFO DEL ARCHIVO 		=> info");
	}

	return 0;
}
