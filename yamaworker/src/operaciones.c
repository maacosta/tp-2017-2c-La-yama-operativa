#include "operaciones.h"

static void guardar_archivo_tmp(const char *path, const char *nombre_archivo, unsigned char *stream, ssize_t len, bool es_txt) {
	if(!global_get_dir_exist(path))
		global_create_dir(path);

	char *path_file = string_from_format("%s/%s", path, nombre_archivo);
	if(es_txt) global_create_txtfile(path_file, stream, len);
	else global_create_binfile(path_file, stream, len);
	free(path_file);
}

bool op_transformar(packet_t *packet, socket_t sockMaster, yamaworker_t* config) {
	header_t cabecera;
	packet_t paquete;
	size_t size;

	log_msg_info("operaciones | Etapa Transformacion: socket master [ %d ]", sockMaster);

	//recibir informacion de bloque y nombre archivo final temporal
	int num_bloque, bytes_ocupados, i_es_txt;
	char nombre_archivo_final_tmp[NOMBRE_ARCHIVO_TMP];
	bool es_txt;
	serial_string_unpack(packet->payload, "h h s h", &num_bloque, &bytes_ocupados, &nombre_archivo_final_tmp, &i_es_txt);
	es_txt = (bool)i_es_txt;
	protocol_packet_free(packet);
	//escribir bloque a disco con un nombre temporal
	char nombre_bloque_tmp[NOMBRE_ARCHIVO_TMP];
	global_nombre_aleatorio(&nombre_bloque_tmp, 6);
	memoria_abrir(config->path_databin, true);
	unsigned char *bloque = memoria_obtener_bloque(num_bloque, bytes_ocupados);
	guardar_archivo_tmp(config->path_tmp, &nombre_bloque_tmp, bloque, bytes_ocupados, true);
	memoria_destruir();

	//recibir archivo temporal y escribirlo a disco con un nombre temporal
	paquete = protocol_packet_receive(sockMaster);
	if(paquete.header.operation == OP_ERROR) {
		return false;
	}
	char nombre_script_tmp[NOMBRE_ARCHIVO_TMP];
	global_nombre_aleatorio(&nombre_script_tmp, 8);
	guardar_archivo_tmp(config->path_tmp, &nombre_script_tmp, paquete.payload, paquete.header.size, es_txt);
	protocol_packet_free(&paquete);

	//ejecutar transformacion
	char *path_bloque = string_from_format("./%s/%s", config->path_tmp, nombre_bloque_tmp);
	char *path_script = string_from_format("./%s/%s", config->path_tmp, nombre_script_tmp);
	char *path_resultado = string_from_format("./%s/%s", config->path_tmp, nombre_archivo_final_tmp);

	//ejecutar archivo temporal ya guardado localmente
	char *cmd = string_from_format("cat %s | %s | sort > %s", path_bloque, path_script, path_resultado);
	log_msg_info("operaciones | Comando a ejecutar [ %s ]", cmd);
	//asignar permisos a script transformacion
	chmod(path_script, S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH);

	resultado_t resultado;
	if(system(cmd) == -1) {
		resultado = RESULTADO_Error;
		log_msg_error("operaciones | No se pudo ejecutar el comando [ %s ]", strerror(errno));
	}
	resultado = RESULTADO_OK;

	free(path_bloque);
	free(path_script);
	free(path_resultado);
	free(cmd);

	//responder a master
	char buffer[RESPUESTA_SIZE];
	size = serial_string_pack(&buffer, "h", resultado);
	cabecera = protocol_get_header(OP_WRK_Iniciar_Transformacion, size);
	paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(sockMaster, &paquete)) {
		return false;
	}

	return true;
}

bool op_reduccion(packet_t *packet, socket_t sockMaster, yamaworker_t* config) {
	header_t cabecera;
	packet_t paquete;
	size_t size;

	log_msg_info("operaciones | Etapa Reduccion: socket master [ %d ]", sockMaster);

	//recibir informacion de archivos reducidos y nombre archivo final temporal
	char nombre_archivos_tmp[NOMBRE_ARCHIVO_TMP*10];
	char nombre_archivo_reduccion_local[NOMBRE_ARCHIVO_TMP];
	bool i_es_txt;
	serial_string_unpack(packet->payload, "s s h", &nombre_archivos_tmp, &nombre_archivo_reduccion_local, &i_es_txt);
	bool es_txt = (bool)i_es_txt;
	protocol_packet_free(packet);
	//escribir archivos reducidos unificado a disco con un nombre temporal
	char *cmd = string_duplicate("cat ");
	char **arc_reduccion_lista = string_split(&nombre_archivos_tmp, TOKEN_SEPARADOR_ARCHIVOS);
	void iterar(char *a) {
		char *path = string_from_format("./%s/%s", config->path_tmp, a);
		string_append_with_format(&cmd, "%s ", path);
		free(path);
		free(a);
	}
	string_iterate_lines(arc_reduccion_lista, (void *)iterar);
	free(arc_reduccion_lista);

	//recibir archivo temporal y escribirlo a disco con un nombre temporal
	paquete = protocol_packet_receive(sockMaster);
	if(paquete.header.operation == OP_ERROR) {
		return false;
	}
	char nombre_script_tmp[NOMBRE_ARCHIVO_TMP];
	global_nombre_aleatorio(&nombre_script_tmp, 8);
	guardar_archivo_tmp(config->path_tmp, &nombre_script_tmp, paquete.payload, paquete.header.size, es_txt);
	protocol_packet_free(&paquete);

	//ejecutar reduccion
	char *path_script = string_from_format("./%s/%s", config->path_tmp, nombre_script_tmp);
	char *path_resultado = string_from_format("./%s/%s", config->path_tmp, nombre_archivo_reduccion_local);

	string_append_with_format(&cmd, "| %s > %s", path_script, path_resultado);
	log_msg_info("operaciones | Comando a ejecutar [ %s ]", cmd);

	//asignar permisos a script reductor
	chmod(path_script, S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH);

	resultado_t resultado;
	if(system(cmd) == -1) {
		resultado = RESULTADO_Error;
		log_msg_error("operaciones | No se pudo ejecutar el comando [ %s ]", strerror(errno));
	}
	resultado = RESULTADO_OK;

	free(path_script);
	free(path_resultado);
	free(cmd);

	//responder a master
	char buffer[RESPUESTA_SIZE];
	size = serial_string_pack(&buffer, "h", resultado);
	cabecera = protocol_get_header(OP_WRK_Iniciar_Reduccion, size);
	paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(sockMaster, &paquete)) {
		return false;
	}

	return true;
}
