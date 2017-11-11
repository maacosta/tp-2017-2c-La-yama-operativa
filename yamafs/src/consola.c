#include "consola.h"

bool fin_consola;

static void comando_ayuda(char **cmd) {
	puts("Comandos:");
	puts(">format");
	puts(" Formatear el file system");
	puts(">rm [path-archivo] | -d [path-directorio] | -b [path-archivo] [nro-bloque] [nro-copia]");
	puts(" Eliminar archivo/directorio/bloque");
	puts(">rename [path-original] [path-modificado]");
	puts(" Renombrar archivo/directorio");
	puts(">mv [path-original] [path-final]");
	puts(" Mover archivo/directorio");
	puts(">cat [path-archivo]");
	puts(" Mostrar contenido del archivo como texto plano");
	puts(">mkdir [path-dir]");
	puts(" Crear directorio");
	puts(">cpfrom [path-archivo-origen] [directorio-yama]");
	puts(" Copiar un archivo local al yamafs");
	puts(">cpto [path-archivo-yamafs] [directorio-filesystem]");
	puts(" Copiar un archivo local desde el yamafs");
	puts(">cpblock [path-archivo] [nro-bloque] [id-nodo]");
	puts(" Crear una copia de un bloque de un archivo en el nodo dado");
	puts(">md5 [path-archivo-yamafs]");
	puts(" Solicitar el MD5 de un archivo en yamafs");
	puts(">ls [path-directorio]");
	puts(" Listar los archivos de un directorio");
	puts(">info [path-archivo]");
	puts(" Mostrar informacion del archivo");
}

static void comando_format(char **cmd) {
	borrar_estructura_de_datos();
	crear_estructura_de_datos();
	puts("Se formateo el file system");
}

static void comando_rm(char **cmd) {
	//contar parametros
	int i = 0;
	void iterar(char *param) {
		i++;
	}
	string_iterate_lines(cmd, (void*)iterar);
	//validar parametros
	if(i != 2 && i != 3 && i != 5) {
		puts("La cantidad de parametros es incorrecta");
		return;
	}

	int rdo;
	switch(i) {
	case 2: //rm path_archivo
		break;
	case 3: //rm -d path_directorio
		if(!string_equals_ignore_case(cmd[1], "-d")) {
			puts("Los parametros son incorrectos");
			return;
		}
		rdo = directorio_borrar_dir(cmd[2]);
		switch(rdo) {
		case 0: puts("Se borro el directorio"); break;
		case -1: puts("El path indicado debe empezar con /"); break;
		case -2: puts("No existe el directorio a borrar"); break;
		case -3: puts("El directorio contiene subdirectorios"); break;
		}
		break;
	case 5: //rm -b path_archivo nro_bloque nro_copia
		break;
	}
}

static void comando_mkdir(char **cmd) {
	char *path_dir;
	//obtener parametros
	int i = 0;
	void iterar(char *param) {
		if(i != 0) {
			if(i == 1) path_dir = param;
		}
		i++;
	}
	string_iterate_lines(cmd, (void*)iterar);
	//validar parametros
	if(i - 1 != 1) {
		puts("La cantidad de parametros es incorrecta");
		return;
	}
	int rdo = directorio_crear_dir(path_dir);
	if(rdo == 0) puts("Se creo el directorio");
	else if(rdo == -1) puts("El path indicado debe empezar con /");
	else if(rdo == -2) puts("No hay directorios disponibles");
	else puts("Ocurrio un error indeterminado creando el directorio");
}

static void comando_salir(char **cmd) {
	fin_consola = true;
	puts("Fin del programa");
}

void consola_iniciar() {
	fin_consola = false;
	char *comando;
	char **cmd;
	puts("Ingrese un comando (ayuda -> para mas informacion; salir -> para terminar el programa)");
	do {
		comando = readline(">");
		cmd = string_split(comando, " ");
		if(string_equals_ignore_case(cmd[0], "ayuda")) comando_ayuda(cmd);
		else if(string_equals_ignore_case(cmd[0], "format")) comando_format(cmd);
		else if(string_equals_ignore_case(cmd[0], "rm")) comando_rm(cmd);
		else if(string_equals_ignore_case(cmd[0], "mkdir")) comando_mkdir(cmd);
		else if(string_equals_ignore_case(cmd[0], "salir")) comando_salir(cmd);
		free(comando);
		string_iterate_lines(cmd, (void*) free);
		free(cmd);
	} while(!fin_consola);
}
