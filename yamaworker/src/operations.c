#include "operations.h"

static yamaworker_t *config;
static socket_t sockWORKER;
static socket_t sockMS;

void operation_init(yamaworker_t* c, socket_t sockworker, socket_t sockms) {
	config = c;
	sockWORKER = sockworker;
	sockMS = sockms;
}

void yw_iniciar_transformacion(packet_t paquete, socket_t cliente){
	int pipe_padreAHijo[2];
	int pipe_hijoAPadre[2];

	pipe(pipe_padreAHijo);
	pipe(pipe_hijoAPadre);

	pid_t pid;
	int status;
	char* buffer=malloc(SIZE);
	if ((pid=fork()) == 0) {
		/*proceso hijo*/

		dup2(pipe_padreAHijo[0],STDIN_FILENO);
		dup2(pipe_hijoAPadre[1],STDOUT_FILENO);

		close( pipe_padreAHijo[1] );
		close( pipe_hijoAPadre[0] );
		close( pipe_hijoAPadre[1]);
		close( pipe_padreAHijo[0]);

		char *argv[] = {NULL};
		char *envp[] = {NULL};
		execve("./script_transformacion.py", argv, envp);
		exit(1);
	}
	else {
		/*proceso padre*/
		close( pipe_padreAHijo[0] ); //Lado de lectura de lo que el padre le pasa al hijo.
		close( pipe_hijoAPadre[1] ); //Lado de escritura de lo que hijo le pasa al padre.

		write( pipe_padreAHijo[1],"hola pepe",strlen("hola pepe"));//ARCHIVO QUE SE RECIBE Y LARGO
		/*Asi de sencillo es escribir en el proceso hijo, cuando el hijo lo reciba, lo va a recibir como
		entrada estandar. En este caso le estoy mandando "hola pepe" al script de python*/

		close( pipe_padreAHijo[1]);
		/*Ya esta, como termine de escribir cierro esta parte del pipe*/

		waitpid(pid,&status,0);

		read( pipe_hijoAPadre[0], buffer, SIZE );
		close( pipe_hijoAPadre[0]);
		/*Listo asi de sencillo leo de un proceso hijo, ahora el resultado de mi script se encuentra en
		"buffer" y tiene un tamaño SIZE. Como termine de leer cierro el extremo del pipe*/
	}

	/*Ya esta, tengo mi resultado en buffer ¿Que puedo hacer con eso? Escribirlo en un archivo por ejemplo ;)*/
	FILE* fd = fopen("/tmp/resultado","w");
	fputs(buffer,fd);
	fclose(fd);

	/*Eso es todo! Fijense que a la hora de reduccion de un archivo es exactamente igual:
	Crean 2 pipes, forkean, corren el script, guardan el resultado.*/
	free(buffer);
}
