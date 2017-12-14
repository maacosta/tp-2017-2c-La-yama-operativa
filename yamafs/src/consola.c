#include "consola.h"

yamafs_t* cfg;
bool fin_consola;
pthread_t thSRV;
extern char comando_global[80];

static const char *md5sum(const char *chaine, size_t len) {
	struct md5_ctx ctx;
    unsigned char digest[16];
    md5_init(&ctx);
    ctx.size = len;
    strcpy(ctx.buf, chaine);
    md5_update(&ctx);
    md5_final(digest, &ctx);

    char md5string[33];
    int i;
    for(i = 0; i < 16; ++i)
        sprintf(&md5string[i*2], "%02x", (unsigned int)digest[i]);

    return string_duplicate((char*)&md5string);
}

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
	puts(">cpfrom [-t | -b] [path-archivo-origen] [directorio-yama]");
	puts(" Copiar un archivo local al yamafs (directorio-yama debe empezar con /)");
	puts(">cpto [path-archivo-yamafs] [directorio-filesystem]");
	puts(" Copiar un archivo local desde el yamafs (directorio-yama debe empezar con /)");
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
	void iterar(char *param) { i++; }
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
	//contar parametros
	int i = 0;
	void iterar(char *param) { i++; }
	string_iterate_lines(cmd, (void*)iterar);
	//validar parametros
	if(i - 1 != 1) {
		puts("La cantidad de parametros es incorrecta");
		return;
	}
	int rdo = directorio_crear_dir(cmd[1]);
	if(rdo == 0) puts("Se creo el directorio");
	else if(rdo == -1) puts("El path indicado debe empezar con /");
	else if(rdo == -2) puts("No hay directorios disponibles");
	else puts("Ocurrio un error indeterminado creando el directorio");
}

static void comando_cpfrom(char **cmd) {
	//obtener parametros
	int i = 0;
	void iterar(char *param) { i++; }
	string_iterate_lines(cmd, (void*)iterar);
	//validar parametros
	if(i - 1 != 3) {
		puts("La cantidad de parametros es incorrecta");
		return;
	}
	bool es_txt;
	if(string_equals_ignore_case(cmd[1], "-b"))
		es_txt = false;
	else if(string_equals_ignore_case(cmd[1], "-t"))
		es_txt = true;
	else {
		puts("Debe indicar el tipo de archivo: -t texto; -b binario");
		return;
	}
	if(!global_get_file_exist(cmd[2])) {
		puts("El archivo origen no existe");
		return;
	}
	char archivo[50];
	int indice = directorio_obtener_indice(cmd[3], &archivo);
	if(indice == -1) {
		puts("El directorio destino no existe");
		return;
	}

	pthread_kill(thSRV, SIGRTMIN + 1);
	puts("Se envio a procesar la solicitud");
}

static void comando_cpto(char **cmd) {
	//obtener parametros
	int i = 0;
	void iterar(char *param) { i++; }
	string_iterate_lines(cmd, (void*)iterar);
	//validar parametros
	if(i - 1 != 2) {
		puts("La cantidad de parametros es incorrecta");
		return;
	}
	if(global_get_file_exist(cmd[2])) {
		puts("Ya existe el archivo en el destino especificado");
		return;
	}
	char archivo[50];
	int indice = directorio_obtener_indice(cmd[1], &archivo);
	if(indice == -1) {
		puts("El directorio origen no existe");
		return;
	}
	if(!archivo_existe_config_nombre(cfg, &archivo, indice)) {
		puts("El archivo origen no existe");
		return;
	}

	pthread_kill(thSRV, SIGRTMIN + 2);
	puts("Se envio a procesar la solicitud");
}

static void comando_md5(char **cmd) {
	//obtener parametros
	int i = 0;
	void iterar(char *param) { i++; }
	string_iterate_lines(cmd, (void*)iterar);
	//validar parametros
	if(i - 1 != 1) {
		puts("La cantidad de parametros es incorrecta");
		return;
	}

	char *path_dir = cmd[1];
	unsigned char *cod = md5sum(cmd[1], string_length(cmd[1]));
	printf("md5 %s\n", cod);
}

static void comando_salir(char **cmd) {
	pthread_kill(thSRV, SIGRTMIN);
	fin_consola = true;
	puts("Finalizando programa...");
}

void consola_iniciar(yamafs_t* config, pthread_t th_srv) {
	cfg = config;
	thSRV = th_srv;
	fin_consola = false;
	char *comando;
	char **cmd;
	puts("Ingrese un comando (ayuda -> para mas informacion; salir -> para terminar el programa)");
	do {
		comando = readline(">");
		cmd = string_split(comando, " ");
		strcpy(&comando_global, comando);
		if(string_equals_ignore_case(cmd[0], "ayuda")) comando_ayuda(cmd);
		else if(string_equals_ignore_case(cmd[0], "format")) comando_format(cmd);
		else if(string_equals_ignore_case(cmd[0], "rm")) comando_rm(cmd);
		else if(string_equals_ignore_case(cmd[0], "mkdir")) comando_mkdir(cmd);
		else if(string_equals_ignore_case(cmd[0], "cpfrom")) comando_cpfrom(cmd);
		else if(string_equals_ignore_case(cmd[0], "cpto")) comando_cpto(cmd);
		else if(string_equals_ignore_case(cmd[0], "md5")) comando_md5(cmd);
		else if(string_equals_ignore_case(cmd[0], "salir")) comando_salir(cmd);
		free(comando);
		string_iterate_lines(cmd, (void*) free);
		free(cmd);
	} while(!fin_consola);
}
