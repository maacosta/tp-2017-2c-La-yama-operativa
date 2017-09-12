#include "consola.h"

static int mostrar_ayuda(char* parametro) {
	if (parametro == NULL) {
		puts("Accion 			=> Comando");
		puts("---------------------	=> -----------------");
		puts("FORMATER 		=> format");
		puts("ELIMINAR ARCHIVO, DIRECTORIO, NODO O BLOQUE	=> rm");
		puts("RENOMBRAR ARCHIVO O DIRECTORIO	=> rename");
		puts("MOVER ARCHIVO O DIRECTORIO	=> mv");
		puts("MOSTRAR ARCHIVO 	=> cat");
		puts("CREAR DIRECTORIO 	=> mkdir");
		puts("COPIAR ARCHIVO LOCAL A FS 	=> cpfrom");
		puts("COPIAR ARCHIVO DE FS A LOCAL 	=> cpto");
		puts("COPIAR BLOQUE 	=> cpblock");
		puts("MD5	=> md5");
		puts("LISTAR ARCHIVOS 			=> ls");
		puts("MUESTRA INFO DEL ARCHIVO 		=> info");
	}

	return 0;
}

static void remueve_salto_de_linea(char* salida, char* texto) {
	strncpy(salida, texto, strlen(texto) - 1);
}

static char* comando_preparado(char* comando) {
	char* cmd_listo = calloc(1, strlen(comando));
	remueve_salto_de_linea(cmd_listo, comando);
	return cmd_listo;
}

static int ejecutar_comando(char* comando) {
	int ret;
	char* cmd_listo = comando_preparado(comando);
	char** parametros = string_n_split(cmd_listo, 6, " ");
	if (string_equals_ignore_case(parametros[0], CMD_AYUDA)) {
		ret = mostrar_ayuda(parametros[1]);
		return ret;
	}else if (string_equals_ignore_case(parametros[0], CMD_FORMATEAR)) {

		return ret;
	}else if (string_equals_ignore_case(parametros[0], CMD_ELIMINAR_ARCHIVO)) {
		if (parametros[1] == NULL) {
			//logearErrorParametrosFaltantes
			return 1;
		}

		return ret;
	}else if (string_equals_ignore_case(parametros[0], CMD_RENOMBRAR_ARCHIVO)) {
		if (parametros[1] == NULL || parametros[2] == NULL) {
			//logearErrorParametrosFaltantes
			return 1;
		}

		return ret;
	}else if (string_equals_ignore_case(parametros[0], CMD_MOVER_ARCHIVO)) {
		if (parametros[1] == NULL || parametros[2] == NULL) {
			//logearErrorParametrosFaltantes
			return 1;
		}

		return ret;
	}else if (string_equals_ignore_case(parametros[0], CMD_MOSTRAR_ARCHIVO)) {
		if (parametros[1] == NULL) {
			//logearErrorParametrosFaltantes
			return 1;
		}

		return ret;
	}else if (string_equals_ignore_case(parametros[0], CMD_COPIAR_ARCHIVO_FS_LOCAL)) {
		if (parametros[1] == NULL || parametros[2] == NULL) {
			//logearErrorParametrosFaltantes
			return 1;
		}

		return ret;
	}else if (string_equals_ignore_case(parametros[0], CMD_COPIAR_ARCHIVO_LOCAL_FS)) {
		if (parametros[1] == NULL || parametros[2] == NULL) {
			//logearErrorParametrosFaltantes
			return 1;
		}

		return ret;
	}else if (string_equals_ignore_case(parametros[0], CMD_CREAR_DIRECTORIO)) {
		if (parametros[1] == NULL) {
			//logearErrorParametrosFaltantes
			return 1;
		}

		return ret;
	}else if (string_equals_ignore_case(parametros[0], CMD_COPIAR_BLOQUE)) {
		if (parametros[1] == NULL || parametros[2] == NULL || parametros[3] == NULL) {
			//logearErrorParametrosFaltantes
			return 1;
		}

		return ret;
	}else if (string_equals_ignore_case(parametros[0], CMD_MD5)) {
		if (parametros[1] == NULL) {
			//logearErrorParametrosFaltantes
			return 1;
		}

		return ret;
	}else if (string_equals_ignore_case(parametros[0], CMD_LISTAR)) {
		if (parametros[1] == NULL) {
			//logearErrorParametrosFaltantes
			return 1;
		}

		return ret;
	}else if (string_equals_ignore_case(parametros[0], CMD_MOSTRAR_INFO_ARCHIVO)) {
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

static void iniciar() {
	//En realidad no importa el tamaño del buffer porque si se pasa getLine se da cuenta y hace un realloc :D
	size_t buffer_size = 100;
	char* comando = (char*)calloc(1, buffer_size);
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
			char* CMD_listo = comando_preparado(comando);
			//log_debug_interno("El comando %s fue ejecutado con exito", CMD_listo);
		}
	}
	free(comando);
}

void consola_crear(yamafs_t *config) {
	pthread_t th_consola;

	pthread_create(&th_consola, NULL, (void*)iniciar, NULL);

	pthread_join(th_consola, NULL);
}
