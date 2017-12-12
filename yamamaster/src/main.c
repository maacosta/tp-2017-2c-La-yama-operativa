#include "main.h"

unsigned char buffer[4096];
yamamaster_t *config;
socket_t sockYAMA;
char *p_tipo_transformador, *p_transformador, *p_tipo_reductor, *p_reductor, *p_origen, *p_destino;
bool p_txt_transformador, p_txt_reductor;

static void imprimir_formato_parametros() {
	puts("El formato aceptado son los siguientes:");
	puts(">yamamaster [-t | -b] [path-transformador] [-t | -b] [path-reductor] [path-origen-txt] [path-destino-txt]");
	puts(">yamamaster [path-transformador-txt] [path-reductor-txt] [path-origen-txt] [path-destino-txt]");
	log_msg_error("Error en formato de parametros");
}

void validar_parametros(int argc, char **argv) {
	if(argc != 7 && argc != 5) {
		imprimir_formato_parametros();
		exit(EXIT_FAILURE);
	}
	if(argc == 7) {
		p_tipo_transformador = argv[1];
		p_transformador = argv[2];
		p_tipo_reductor = argv[3];
		p_reductor = argv[4];
		p_origen = argv[5];
		p_destino = argv[6];
	}
	else if(argc == 5) {
		p_tipo_transformador = string_duplicate("-t");
		p_transformador = argv[1];
		p_tipo_reductor = string_duplicate("-t");
		p_reductor = argv[2];
		p_origen = argv[3];
		p_destino = argv[4];
	}
	if(!string_equals_ignore_case(p_tipo_transformador, "-t") && !string_equals_ignore_case(p_tipo_transformador, "-b")
		&& !string_equals_ignore_case(p_tipo_reductor, "-t") && !string_equals_ignore_case(p_tipo_reductor, "-b")) {
		imprimir_formato_parametros();
		exit(EXIT_FAILURE);
	}
	p_txt_transformador = string_equals_ignore_case(p_tipo_transformador, "-t");
	p_txt_reductor = string_equals_ignore_case(p_tipo_reductor, "-t");
	log_msg_info("Parametros %s %s %s %s %s %s", p_tipo_transformador, p_transformador, p_tipo_reductor, p_reductor, p_origen, p_destino);
}

socket_t conectar_con_yama(yamamaster_t *config) {
	socket_t sock;
	if((sock = socket_connect(config->yama_ip, config->yama_puerto)) == -1) {
		exit(EXIT_FAILURE);
	}

	if(!protocol_handshake_send(sock)) {
		exit(EXIT_FAILURE);
	}
	header_t header;
	if(!protocol_handshake_receive(sock, &header)) {
		exit(EXIT_FAILURE);
	}
	return sock;
}

socket_t conectar_con_worker(char *ip, char *puerto) {
	socket_t sock;
	if((sock = socket_connect(ip, puerto)) == -1) {
		return -1;
	}

	if(!protocol_handshake_send(sock)) {
		socket_close(sock);
		return -1;
	}
	header_t header;
	if(!protocol_handshake_receive(sock, &header)) {
		socket_close(sock);
		return -1;
	}
	return sock;
}

int main(int argc, char **argv) {
	global_set_process(MASTER);
	config = config_leer("metadata");
	log_init(config->log_file, config->log_name, true);
	validar_parametros(argc, argv);

	sockYAMA = conectar_con_yama(config);

	ejecutar_transformacion(sockYAMA, p_txt_transformador, p_transformador, p_origen);

	ejecutar_reduccion(sockYAMA, p_txt_reductor, p_reductor);

	ejecutar_reduccion_global(sockYAMA, p_txt_reductor, p_reductor);

	ejecutar_almacenamiento(sockYAMA, p_destino);

	config_liberar(config);

	return EXIT_SUCCESS;
}
