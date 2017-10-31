#include "operations.h"

static yamaworker_t *config;
static socket_t sockWORKER;
static socket_t sockMS;


void operation_init(yamaworker_t* c, socket_t sockworker, socket_t sockms) {
	config = c;
	sockWORKER = sockworker;
	sockMS = sockms;
}


char *obtener_contenido_databin(int bloque){
	return "hola pepe";
}

/*a la hora de reduccion de un archivo es exactamente igual:
Crean 2 pipes, forkean, corren el script, guardan el resultado.*/
int yw_operations(packet_t script, socket_t cliente){
	packet_t info;
	info = protocol_packet_receive(cliente);
	if(info.header.operation == OP_ERROR) {
		socket_close(cliente);
		return(EXIT_FAILURE);
	}
	int pipe_padreAHijo[2];
	int pipe_hijoAPadre[2];

	pipe(pipe_padreAHijo);
	pipe(pipe_hijoAPadre);

	pid_t pid;
	int status;
	char* buffer=malloc(SIZE);
	char nombre_archivo_tmp;
	if ((pid=fork()) == 0) {
		/*proceso hijo*/

		dup2(pipe_padreAHijo[0],STDIN_FILENO);
		dup2(pipe_hijoAPadre[1],STDOUT_FILENO);

		close(pipe_padreAHijo[1]);
		close(pipe_hijoAPadre[0]);
		close(pipe_hijoAPadre[1]);
		close(pipe_padreAHijo[0]);

		char *argv[] = {NULL};
		char *envp[] = {NULL};

		unsigned char *programa = script.payload;
		//necesito guardarlo local en un archivo y en el execve va el path
		execve(programa, argv, envp);
		exit(1);
	}
	else {
		/*proceso padre*/
		close(pipe_padreAHijo[0]); //Lado de lectura de lo que el padre le pasa al hijo.
		close(pipe_hijoAPadre[1]); //Lado de escritura de lo que hijo le pasa al padre.

		int bloque=0;
		int bytes_ocupados=0;
		//char nombre_archivo_tmp;
		serial_string_unpack(info.payload, "h h s", &bloque, &bytes_ocupados, &nombre_archivo_tmp);

		char *informacion = obtener_contenido_databin(bloque);

		write(pipe_padreAHijo[1],informacion,strlen(informacion));//ARCHIVO QUE SE RECIBE Y LARGO
		/* Asi de sencillo es escribir en el proceso hijo, cuando el hijo lo reciba, lo va a recibir como
		entrada estandar. En este caso le estoy mandando "hola pepe" al script de python*/

		close(pipe_padreAHijo[1]);
		/*Ya esta, como termine de escribir cierro esta parte del pipe*/

		waitpid(pid,&status,0);

		read(pipe_hijoAPadre[0], buffer, SIZE );
		close(pipe_hijoAPadre[0]);
		/*Listo asi de sencillo leo de un proceso hijo, ahora el resultado de mi script se encuentra en
		"buffer" y tiene un tamaño SIZE. Como termine de leer cierro el extremo del pipe*/
	}
	/*Ya esta, tengo mi resultado en buffer ¿Que puedo hacer con eso? Escribirlo en un archivo por ejemplo ;)*/
	char *nom_arc = string_new();
	string_append(&nom_arc, "/tmp/");
	string_append(&nom_arc, nombre_archivo_tmp);
	//FILE* fd = fopen(string_append("/tmp/", nombre_archivo_tmp),"w");
	FILE* fd = fopen(nom_arc,"w");
	fputs(buffer,fd);
	fclose(fd);

	/*Eso es todo! Fijense que a la hora de reduccion de un archivo es exactamente igual:
	Crean 2 pipes, forkean, corren el script, guardan el resultado.*/
	free(buffer);
	return (EXIT_SUCCESS);
}
