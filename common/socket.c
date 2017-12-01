#include "socket.h"

#define SOCKET_BACKLOG 5
#define BUFFER_CAPACITY 1024
#define BUFFER_PACKET 1452

static struct addrinfo *create_addrinfo(const char *ip, const char *port) {
	if(port != NULL) {
		int n = atoi(port);
		if(n < 1024 && n > 65535) {
			log_msg_error("socket | El numero [ %d ] de puerto debe estar entre 1024 y 65535", n);
			exit(EXIT_FAILURE);
		}
	}

	struct addrinfo hints, *addr;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	if(ip == NULL) hints.ai_flags = AI_PASSIVE;

	int status = getaddrinfo(ip, port, &hints, &addr);
	if(status != 0) {
		log_msg_error("socket | %s", gai_strerror(status));
		exit(EXIT_FAILURE);
	}

	return addr;
}

static socket_t create_socket(struct addrinfo *addr) {
	socket_t sockfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);

	if(sockfd != -1) {
		int reuse = 1;
		setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof reuse);
	}

	return sockfd;
}

socket_t socket_init(const char *ip, const char *port) {
	struct addrinfo *cur, *addr = create_addrinfo(ip, port);
	socket_t sockfd = -1;
	int ret = -1;

	for(cur = addr; cur != NULL; cur = cur->ai_next) {
		sockfd = create_socket(cur);
		if(sockfd == -1) continue;
		log_msg_info("socket | [ %d ] creado", sockfd);

		if(ip == NULL) {
			ret = bind(sockfd, cur->ai_addr, cur->ai_addrlen);
		} else {
			ret = connect(sockfd, cur->ai_addr, cur->ai_addrlen);
		}

		if(ret == -1) {
			socket_close(sockfd);
			continue;
		}

		if(ip == NULL) {
			log_msg_info("socket | Enlazado con puerto [ %s ]", port);
		} else {
			log_msg_info("socket | Conectado a [ %s:%s ]", ip, port);
		}
		break;
	}

	freeaddrinfo(addr);
	if(sockfd == -1) {
		log_msg_error("socket | No se pudo crear el socket");
	}
	if(ret == -1) {
		log_msg_error("socket | No se pudo conectar/bindear el socket");
		sockfd = ret;
	}

	if(ip == NULL) {
		if(listen(sockfd, SOCKET_BACKLOG) == -1) {
			log_msg_error("socket | Fallo la escucha en el puerto [ %s ] %s", port, strerror(errno));
			exit(EXIT_FAILURE);
		}
		log_msg_info("socket | Escuchando en el puerto [ %s ]", port);
	}

	return sockfd;
}

static fd_set active_fdset;
static socket_t fdmax;

socket_t socket_listen(const char *port) {
	socket_t sock = socket_init(NULL, port);
	FD_ZERO(&active_fdset);
	FD_SET(sock, &active_fdset);
	fdmax = sock;
	return sock;
}

bool socket_select(fd_set *read_fdset) {
	*read_fdset = active_fdset;
	if(select(fdmax + 1, read_fdset, NULL, NULL, NULL) == -1) {
		log_msg_error("socket | %s", strerror(errno));
		return false;
	}
	return true;
}

int socket_fdmax() {
	return fdmax;
}

bool socket_fdisset(socket_t sock, fd_set *fd) {
	return FD_ISSET(sock, fd);
}

void socket_fdset(socket_t sock) {
	FD_SET(sock, &active_fdset);
	fdmax = fdmax > sock ? fdmax : sock;
}

void socket_fdclear(socket_t sock) {
	FD_CLR(sock, &active_fdset);
}

socket_t socket_accept(socket_t sv_sock) {
	struct sockaddr rem_addr;
	socklen_t addr_size = sizeof rem_addr;

	socket_t cli_sock = accept(sv_sock, &rem_addr, &addr_size);

	struct sockaddr_in *addr_in = (struct sockaddr_in*)&rem_addr;
	char remote_ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &addr_in->sin_addr, remote_ip, INET_ADDRSTRLEN);

	if(cli_sock != -1) {
		log_msg_info("socket | Cliente [ %s ] conectado al socket [ %d ]", remote_ip, cli_sock);
	}
	else {
		log_msg_error("socket | Error al aceptar el cliente desde [ %s ]", remote_ip);
	}

	return cli_sock;
}

socket_t socket_connect(const char *ip, const char *port) {
	return socket_init(ip, port);
}

static size_t sendall(socket_t sockfd, unsigned char *buf, size_t len) {
	size_t bytes_sent = 0, bytes_packet;
	while(bytes_sent < len) {
		bytes_packet = len - bytes_sent < BUFFER_PACKET ? len - bytes_sent : BUFFER_PACKET;
		ssize_t n = send(sockfd, buf + bytes_sent, bytes_packet, 0);
		if(n == -1) {
			log_msg_error("socket | Error al enviar por el socket [ %d ], se envio [ %d/%d ] %s", sockfd, bytes_sent, len, strerror(errno));
			return n;
		}
		bytes_sent += n;
	}
	return bytes_sent;
}

size_t socket_send_bytes(unsigned char *message, size_t size, socket_t sockfd) {
	size_t bytes_sent = sendall(sockfd, message, size);
	if(bytes_sent > 0)
		;//log_inform("Sent %ld bytes", bytes_sent);
	return bytes_sent;
}

static ssize_t recvall(socket_t sockfd, unsigned char *buf, size_t len) {
	size_t bytes_received = 0, bytes_packet;
	while(bytes_received < len) {
		bytes_packet = len - bytes_received < BUFFER_PACKET ? len - bytes_received : BUFFER_PACKET;
		ssize_t n = recv(sockfd, buf + bytes_received, bytes_packet, 0);
		if(n == -1) {
			log_msg_error("socket | Ocurrio un error al recibir sobre el socket [ %d ], se recibio [ %d/%d ] %s", sockfd, bytes_received, len, strerror(errno));
			return n;
		}
		if(n == 0) {
			log_msg_error("socket | La conexion sobre el socket [ %d ] se cerro", sockfd);
			return n;
		}
		bytes_received += n;
	}
	return bytes_received;
}

ssize_t socket_receive_bytes(unsigned char *message, size_t size, socket_t sockfd) {
	ssize_t bytes_received = recvall(sockfd, message, size);
	if(bytes_received > 0)
		;//log_inform("Received %ld bytes", bytes_received);
	return bytes_received;
}

void socket_close(socket_t sockfd) {
	if(shutdown(sockfd, SHUT_RDWR) == -1) {
		log_msg_error(strerror(errno));
	}
	int res = close(sockfd);
	if(res != 1) {
		log_msg_info("socket | [ %d ] cerrado", sockfd);
	}
}
