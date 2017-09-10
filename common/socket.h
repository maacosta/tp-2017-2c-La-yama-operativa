#ifndef socket_h
#define socket_h

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <poll.h>
#include <errno.h>
#include "log.h"

typedef int socket_t;

typedef struct {
    fd_set set;
    socket_t max;
} fdset_t;

/**
 * Si se indica la IP, crea un socket y lo conecta al servidor de la IP y
 * puerto especificados. Si no se indica la IP, crea un socket y lo prepara
 * para escuchar conexiones en el puerto especificado.
 * @param ip Dirección IP del servidor.
 * @param port Puerto.
 * @return Descriptor del socket.
 */
socket_t socket_init(const char *ip, const char *port);

/**
 * Crea un socket de servidor para conectarse con un cliente a través de un
 * puerto determinado.
 * @param port Puerto de escucha.
 * @return Descriptor del socket del cliente.
 */
socket_t socket_listen(const char *port);

/*
 * Función bloqueante que espera por conexiones en un socket servidor y las
 * acepta devolviendo el socket cliente.
 * @param sv_sock Descriptor del socket del servidor.
 * @return Descriptor del socket del cliente (-1 si hubo error).
 */
socket_t socket_accept(socket_t sv_sock);

/**
 * Crea un socket de cliente para conectarse con un servidor en una dirección
 * IP y un puerto determinados.
 * @param ip Dirección IP del servidor.
 * @param port Puerto del servidor.
 * @return Descriptor del socket del servidor.
 */
socket_t socket_connect(const char *ip, const char *port);

/**
 * Envía una cadena de texto por una conexión abierta en un determinado socket.
 * @param message Mensaje a enviar.
 * @param sockfd Descriptor del socket.
 * @return Número de bytes enviados.
 */
size_t socket_send_string(const char *message, socket_t sockfd);

/**
 * Envía datos binarios por una conexión abierta en un determinado socket.
 * @param message Mensaje a enviar.
 * @param size Tamaño de los datos.
 * @param sockfd Descriptor del socket.
 * @return Número de bytes enviados.
 */
size_t socket_send_bytes(const unsigned char *message, size_t size, socket_t sockfd);

/**
 * Recibe una cadena de texto por una conexión abierta en un determinado socket.
 * @param message Mensaje a recibir.
 * @param sockfd Descriptor del socket.
 * @return Número de bytes recibidos (-1 si hubo error).
 */
ssize_t socket_receive_string(char *message, socket_t sockfd);

/**
 * Recibe datos binarios por una conexión abierta en un determinado socket.
 * @param message Mensaje a recibir.
 * @param size Tamaño de los datos.
 * @param sockfd Descriptor del socket.
 * @return Número de bytes recibidos (-1 si hubo error).
 */
ssize_t socket_receive_bytes(unsigned char *message, size_t size, socket_t sockfd);

/**
 * Crea un conjunto de sockets para ser usado por socket_select().
 * @return Conjunto de sockets.
 */
fdset_t socket_set_create(void);

/**
 * Agrega un socket a un conjunto de sockets.
 * @param fd socket a agregar.
 * @param fds conjunto de sockets.
 */
void socket_set_add(socket_t fd, fdset_t *fds);

/**
 * Cierra un socket abierto con socket_listen() o socket_connect().
 * @param sockfd Descriptor del socket a cerrar.
 */
void socket_close(socket_t sockfd);

#endif /* socket_h */
