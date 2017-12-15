#include "server.h"

pthread_t thSRV;
socket_t sockSRV;
static bool estado_estable;
static bool yama_conectada;
static bool esperar_DNs;
extern char comando_global[80];

static socket_t aceptar_cliente(socket_t server) {
	socket_t cliente = socket_accept(server);
	if(cliente == -1) return -1;

	header_t cabecera;
	if(!protocol_handshake_receive(cliente, &cabecera)) {
		socket_close(cliente);
		return -1;
	}
	//no está en estado-estable -> solo permitir conectar datanodes
	if(!estado_estable && cabecera.process != DATANODE) {
		log_msg_error("No se encuentra en estado-estable, por lo que solo se permiten DataNodes");
		socket_close(cliente);
		return -1;
	}
	//está en estado-estable -> permitir conectar yama y datanodes
	if(estado_estable && !yama_conectada && cabecera.process != DATANODE && cabecera.process != YAMA) {
		log_msg_error("Se encuentra en estado-estable, por lo que solo se permiten DataNodes y YAMA");
		socket_close(cliente);
		return -1;
	}
	//está en estado-estable y yama conectado -> solo permitir conectar workers
	if(estado_estable && yama_conectada && cabecera.process != WORKER) {
		log_msg_error("Se encuentra en estado-estable y YAMA ya está conectado, por lo que solo se permiten Workers");
		socket_close(cliente);
		return -1;
	}
	if(!protocol_handshake_send(cliente)) {
		socket_close(cliente);
		return -1;
	}

	if(cabecera.process == YAMA)
		yama_conectada = true;

	return cliente;
}

static bool procesar_operaciones(socket_t cliente, yamafs_t *config) {
	packet_t packet = protocol_packet_receive(cliente);
	if(packet.header.operation == OP_ERROR) {
		if(packet.header.process == YAMA)
			yama_conectada = false;
		return false;
	}
	bool resultado;
	if(packet.header.process == YAMA) {
		switch(packet.header.operation) {
		case OP_FSY_Informacion_Archivo:
			resultado = filesystem_obtener_datos_archivo(&packet, cliente, config);
			break;
		case OP_FSY_Obtener_Nodos:
			resultado = nodos_informar(&packet, cliente);
			break;
		default:
			log_msg_error("Operacion [ %d ] YAMA no contemplada en el contexto de ejecucion", packet.header.operation);
			protocol_packet_free(&packet);
			return false;
		}
	}
	else if(packet.header.process == DATANODE) {
		switch(packet.header.operation) {
		case OP_FSY_Registrar_Nodo:
			resultado = nodos_registrar(&packet, cliente, config, &esperar_DNs, &estado_estable);
			break;
		default:
			log_msg_error("Operacion [ %d ] DATANODE no contemplada en el contexto de ejecucion", packet.header.operation);
			protocol_packet_free(&packet);
			return false;
		}
	}
	else if(packet.header.process == WORKER) {
		switch(packet.header.operation) {
		case OP_FSY_Almacenar_Archivo:
			resultado = filesystem_almacenamiento_final(&packet, cliente, config);
			break;
		default:
			log_msg_error("Operacion [ %d ] WORKER no contemplada en el contexto de ejecucion", packet.header.operation);
			protocol_packet_free(&packet);
			return false;
		}
	}
	if(!resultado) {
		if(packet.header.process == YAMA)
			yama_conectada = false;
		socket_close(cliente);
	}

	log_msg_info("Procesamiento de operacion [ %d ] [ %s ]", packet.header.operation, resultado ? "EXITOSO" : "FALLIDO");

	return resultado;
}

static void comando_cpfrom(char **cmd, yamafs_t *config) {
	bool es_txt;
	if(string_equals_ignore_case(cmd[1], "-b"))
		es_txt = false;
	else if(string_equals_ignore_case(cmd[1], "-t"))
		es_txt = true;

	char archivo[NOMBRE_ARCHIVO];
	int indice = directorio_obtener_indice(cmd[3], &archivo);

	if(!filesystem_cpfrom(cmd[2], &archivo, indice, es_txt, config)) {
		puts("No se pudo copiar el archivo a yama (ver el log de errores)");
		return;
	}

	puts("   #server @cpfrom: El archivo se grabo en yama con exito");
}

static void comando_cpto(char **cmd, yamafs_t *config) {
	char archivo[50];
	int indice = directorio_obtener_indice(cmd[1], &archivo);

	if(!filesystem_cpto(cmd[2], &archivo, indice, config)) {
		puts("   #server @cpto: No se pudo copiar el archivo a yama (ver el log de errores)");
		return;
	}

	puts("   #server @cpto: El archivo se recupero de yama con exito");
}

static const char *md5sum(const char *chaine, size_t len) {
	struct md5_ctx ctx;
    unsigned char digest[16];
    md5_init(&ctx, len);
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

static void comando_md5(char **cmd, yamafs_t *config) {
	char archivo[50];
	int indice = directorio_obtener_indice(cmd[1], &archivo);

	char nombre_arc[20];
	global_nombre_aleatorio("md5_", &nombre_arc, 12);
	if(!filesystem_cpto(&nombre_arc, &archivo, indice, config)) {
		puts("   #server @cpto: No se pudo recuperar el archivo de yama (ver el log de errores)");
		return;
	}

	ssize_t size;
	unsigned char *stream = global_read_txtfile(&nombre_arc, &size);
	unsigned char *cod = md5sum(stream, size);
	printf("   #server @md5 %s\n", cod);
	free(cod);
	free(stream);
}

static void atender_senial(int sfd, yamafs_t *config, bool *ejecutar) {
	log_msg_info("SE ATIENDE SENIAL");
	struct signalfd_siginfo si;
	ssize_t res;
	res = read (sfd, &si, sizeof(si));
	if (res < 0) {
		log_msg_error("No se pudo leer el fd de la senial %s", strerror(errno));
		return;
	}
	if (res != sizeof(si)) {
		log_msg_error("No se pudo leer la cantidad correcto del fd de la senial");
		return;
	}

	char **cmd = string_split(&comando_global, " ");
	if (si.ssi_signo == SIGRTMIN) { //SALIR
		*ejecutar = false;
	}
	else if (si.ssi_signo == SIGRTMIN + 1) { //CPFROM
		comando_cpfrom(cmd, config);
	}
	else if (si.ssi_signo == SIGRTMIN + 2) { //CPTO
		comando_cpto(cmd, config);
	}
	else if (si.ssi_signo == SIGRTMIN + 3) { //MD5
		comando_md5(cmd, config);
	}
	else {
		log_msg_error("Se envio una senial no controlada");
	}
	putchar('>');
	free(cmd);
}

static int setup_signalfd(void) {
    sigset_t sigs;
    sigemptyset(&sigs);
    sigaddset(&sigs, SIGRTMIN);
    sigaddset(&sigs, SIGRTMIN + 1);
    sigaddset(&sigs, SIGRTMIN + 2);
    sigaddset(&sigs, SIGRTMIN + 3);
    sigprocmask(SIG_BLOCK, &sigs, NULL);
    return signalfd(-1, &sigs, SFD_NONBLOCK | SFD_CLOEXEC);
}

void server_crear(yamafs_t *config) {
	socket_t cli_i;
	fd_set read_fdset;

	sockSRV = socket_listen(config->puerto, "YAMAFS");

	int sfd = setup_signalfd();
	socket_fdset(sfd);

	bool ejecutar = true;
	while(ejecutar) {
		if(!socket_select(&read_fdset)) break;

		for(cli_i = 0; cli_i <= socket_fdmax(); cli_i++) {
			if(!socket_fdisset(cli_i, &read_fdset)) continue;

			if(socket_fdisset(sfd, &read_fdset))
				atender_senial(sfd, config, &ejecutar);
			else if(cli_i == sockSRV) {
				socket_t cli_sock = aceptar_cliente(sockSRV);
				if(cli_sock == -1) continue;
				socket_fdset(cli_sock);
			}
			else {
				if(!procesar_operaciones(cli_i, config)) {
					socket_fdclear(cli_i);
					continue;
				}
			}
		}
	}
	socket_close(sockSRV);
	pthread_exit(0);
}

pthread_t server_crear_fs(yamafs_t *config, bool esperarDNs) {
	esperar_DNs = esperarDNs;
	estado_estable = false;
	yama_conectada = false;
	nodos_inicializar();
	thSRV = thread_create(server_crear, config);
	return thSRV;
}

void server_liberar() {
	thread_join(thSRV);
	nodos_destruir();
}
