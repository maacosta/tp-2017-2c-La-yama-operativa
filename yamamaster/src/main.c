#include "main.h"

unsigned char buffer[4096];
yamamaster_t *config;
socket_t sockYAMA;
char *p_transformador, *p_reductor, *p_origen, *p_destino;

void validar_parametros(int argc, char **argv) {
	if(argc != 5) {
		log_msg_error("Debe indicar 4 parametros: transformador reductor origen destino");
		exit(EXIT_FAILURE);
	}
	p_transformador = argv[1];
	p_reductor = argv[2];
	p_origen = argv[3];
	p_destino = argv[4];
	log_msg_info("Parametros %s %s %s %s", p_transformador, p_reductor, p_origen, p_destino);
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

/*
 * @return 0 No iniciado (no debería darse) | 1 Ok | 2 Replanifica | 3 Error
 */
int verificar_etapa(int numero_etapa) {
	char nombre_archivo[NOMBRE_ARCHIVO];

	header_t cabecera;
	packet_t paquete;
	size_t size;

	//enviar Solicitar Transformacion
	size = serial_string_pack(nombre_archivo, "s", p_origen);
	cabecera = protocol_get_header(OP_YAM_Verificar_Estado, size);
	paquete = protocol_get_packet(cabecera, &nombre_archivo);
	if(!protocol_packet_send(sockYAMA, &paquete))
		exit(EXIT_FAILURE);

	//recibir Solicitar Transformacion
	paquete = protocol_packet_receive(sockYAMA);
	if(paquete.header.operation == OP_ERROR)
		exit(EXIT_FAILURE);
	int etapa, estado;
	serial_string_unpack(paquete.payload, "h h", &etapa, &estado);
	protocol_packet_free(&paquete);

	return numero_etapa == etapa ? estado : 3;
}

int main(int argc, char **argv) {
	global_set_process(MASTER);
	validar_parametros(argc, argv);
	config = config_leer("metadata");
	log_init(config->log_file, config->log_name, true);

	sockYAMA = conectar_con_yama(config);

	ejecutar_transformacion(sockYAMA, p_transformador, p_origen);
	int rdo_etapa = verificar_etapa(1);
	if(rdo_etapa == 2) {
		ejecutar_transformacion(sockYAMA, p_transformador, p_origen);
		if(verificar_etapa(1) != 1)
			exit(EXIT_FAILURE);
	}
	else if(rdo_etapa != 1)
		exit(EXIT_FAILURE);

	ejecutar_reduccion(sockYAMA, p_reductor, p_origen);
	if(verificar_etapa(2) != 1)
		exit(EXIT_FAILURE);

	ejecutar_reduccion_global(sockYAMA, p_reductor, p_origen);
	if(verificar_etapa(3) != 1)
		exit(EXIT_FAILURE);

	ejecutar_almacenamiento(sockYAMA, p_destino, p_origen);
	if(verificar_etapa(4) != 1)
		exit(EXIT_FAILURE);

	return EXIT_SUCCESS;
}
