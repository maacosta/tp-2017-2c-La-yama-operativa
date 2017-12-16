#include "operaciones.h"

bool op_transformar(packet_t *packet, socket_t sockMaster, yamaworker_t* config) {
	header_t cabecera;
	packet_t paquete;
	size_t size;

	log_msg_info("operaciones | Etapa Transformacion: Socket Master [ %d ]", sockMaster);

	//recibir informacion de bloque y nombre archivo final temporal
	int num_bloque, bytes_ocupados, i_es_txt;
	char nombre_archivo_final_tmp[NOMBRE_ARCHIVO_TMP];
	bool es_txt;
	serial_string_unpack(packet->payload, "h h s h", &num_bloque, &bytes_ocupados, &nombre_archivo_final_tmp, &i_es_txt);
	es_txt = (bool)i_es_txt;
	protocol_packet_free(packet);
	//escribir bloque a disco con un nombre temporal
	char nombre_bloque_tmp[NOMBRE_ARCHIVO_TMP];
	global_nombre_aleatorio("wk_t_", &nombre_bloque_tmp, 8);
	memoria_abrir(config->path_databin, true);
	unsigned char *bloque = memoria_obtener_bloque(num_bloque, bytes_ocupados);
	guardar_archivo_tmp(config->path_tmp, (char*)&nombre_bloque_tmp, bloque, bytes_ocupados, true);
	memoria_destruir();

	//recibir archivo temporal y escribirlo a disco con un nombre temporal
	paquete = protocol_packet_receive(sockMaster);
	if(paquete.header.operation == OP_ERROR) {
		return false;
	}
	char nombre_script_tmp[NOMBRE_ARCHIVO_TMP];
	global_nombre_aleatorio("sc_t_", &nombre_script_tmp, 8);
	guardar_archivo_tmp(config->path_tmp, (char*)&nombre_script_tmp, paquete.payload, paquete.header.size, es_txt);
	protocol_packet_free(&paquete);

	//ejecutar transformacion
	char *path_bloque = string_from_format("./%s/%s", config->path_tmp, nombre_bloque_tmp);
	char *path_script = string_from_format("./%s/%s", config->path_tmp, nombre_script_tmp);
	char *path_resultado = string_from_format("./%s/%s", config->path_tmp, nombre_archivo_final_tmp);

	//ejecutar archivo temporal ya guardado localmente
	char *cmd = string_from_format("cat %s | %s | sort > %s", path_bloque, path_script, path_resultado);
	log_msg_info("operaciones | Socket Master [ %d ] Comando a ejecutar [ %s ]", sockMaster, cmd);
	//asignar permisos a script transformacion
	chmod(path_script, S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH);

	resultado_t resultado;
	if(system(cmd) == -1) {
		resultado = RESULTADO_Error;
		log_msg_error("operaciones | T Socket Master [ %d ] No se pudo ejecutar el comando [ %s ]", sockMaster, strerror(errno));
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

	log_msg_info("operaciones | Etapa Reduccion: Socket Master [ %d ]", sockMaster);

	//recibir informacion de archivos reducidos y nombre archivo final temporal
	char nombre_archivos_tmp[NOMBRE_ARCHIVO_TMP*200];
	char nombre_archivo_reduccion_local[NOMBRE_ARCHIVO_TMP];
	int i_es_txt;
	serial_string_unpack((char*)packet->payload, "s s h", &nombre_archivos_tmp, &nombre_archivo_reduccion_local, &i_es_txt);
	bool es_txt = (bool)i_es_txt;
	protocol_packet_free(packet);

	//escribir resultado de apareo en base a la lista de nombre de archivos temporales de transformacion
	char nombre_apareo_tmp[NOMBRE_ARCHIVO_TMP];
	global_nombre_aleatorio("a_r_", &nombre_apareo_tmp, 8);
	char **arc_reduccion_lista = string_split(&nombre_archivos_tmp, TOKEN_SEPARADOR_ARCHIVOS);
	if(!apareo_realizar(arc_reduccion_lista, &nombre_apareo_tmp, config->path_tmp))
		return false;
	free(arc_reduccion_lista);

	//recibir archivo temporal y escribirlo a disco con un nombre temporal
	paquete = protocol_packet_receive(sockMaster);
	if(paquete.header.operation == OP_ERROR) {
		return false;
	}
	char nombre_script_tmp[NOMBRE_ARCHIVO_TMP];
	global_nombre_aleatorio("sc_r_", &nombre_script_tmp, 8);
	guardar_archivo_tmp(config->path_tmp, (char*)&nombre_script_tmp, paquete.payload, paquete.header.size, es_txt);
	protocol_packet_free(&paquete);

	//ejecutar reduccion
	char *path_apareo = string_from_format("%s%s", config->path_tmp, &nombre_apareo_tmp);
	char *path_script = string_from_format("%s%s", config->path_tmp, &nombre_script_tmp);
	char *path_resultado = string_from_format("%s%s", config->path_tmp, &nombre_archivo_reduccion_local);

	char *cmd = string_new();
	string_append_with_format(&cmd, "cat %s | %s > %s", path_apareo, path_script, path_resultado);
	log_msg_info("operaciones | R Socket Master [ %d ] Comando a ejecutar [ %s ]", sockMaster, cmd);

	//asignar permisos a script reductor
	chmod(path_script, S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH);

	resultado_t resultado;
	if(system(cmd) == -1) {
		resultado = RESULTADO_Error;
		log_msg_error("operaciones | R Socket Master [ %d ] No se pudo ejecutar el comando [ %s ]", sockMaster, strerror(errno));
	}
	resultado = RESULTADO_OK;

	free(path_apareo);
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

bool op_reduccion_global(packet_t *packet, socket_t sockMaster, yamaworker_t* config) {
	header_t cabecera;
	packet_t paquete;
	size_t size;

	log_msg_info("operaciones | Etapa Reduccion Global: Socket Master [ %d ]", sockMaster);

	//recibir nombre archivo final temporal y cantidad de reducciones
	int cant_reducciones;
	char nombre_archivo_reduccion_global[NOMBRE_ARCHIVO_TMP];
	bool i_es_txt;
	serial_string_unpack(packet->payload, "s h h", &nombre_archivo_reduccion_global, &cant_reducciones, &i_es_txt);
	bool es_txt = (bool)i_es_txt;
	protocol_packet_free(packet);

	//recibir archivo temporal y escribirlo a disco con un nombre temporal
	paquete = protocol_packet_receive(sockMaster);
	if(paquete.header.operation == OP_ERROR)
		return false;
	char nombre_script_tmp[NOMBRE_ARCHIVO_TMP];
	global_nombre_aleatorio("sc_rg_", &nombre_script_tmp, 8);
	guardar_archivo_tmp(config->path_tmp, (char*)&nombre_script_tmp, paquete.payload, paquete.header.size, es_txt);
	protocol_packet_free(&paquete);

	//recorrer detalle de cada nodo
	char *nombres_arc = malloc(NOMBRE_ARCHIVO_TMP*200);
	t_list *det_reducciones = list_create();
	reduccion_worker_t *det;
	int i, len_tmp = 0;
	for(i = 0; i < cant_reducciones; i++) {
		paquete = protocol_packet_receive(sockMaster);
		if(paquete.header.operation == OP_ERROR)
			return false;

		det = malloc(sizeof(reduccion_worker_t));
		serial_string_unpack((char*)paquete.payload, "s s s", &det->ip, &det->puerto, &det->nombre_archivo_local);
		protocol_packet_free(&paquete);


		strcpy(nombres_arc + len_tmp, TOKEN_SEPARADOR_ARCHIVOS);
		len_tmp += strlen(TOKEN_SEPARADOR_ARCHIVOS);
		strcpy(nombres_arc + len_tmp, &det->nombre_archivo_local);
		len_tmp += strlen(&det->nombre_archivo_local);

		log_msg_info("operaciones | Lista de archivos a aparear [ %s ] len [ %d ]", nombres_arc, len_tmp);
		//string_append_with_format(&nombres_arc, "%s%s", TOKEN_SEPARADOR_ARCHIVOS, &det->nombre_archivo_local);

		list_add(det_reducciones, det);
	}
	log_msg_info("operaciones | Lista de archivos a aparear [ %s ]", nombres_arc);

	//conectar con workers y recuperar archivo de reduccion local
	for(i = 0; i < list_size(det_reducciones); i++) {
		reduccion_worker_t *det = list_get(det_reducciones, i);

		if(string_equals_ignore_case((char*)&det->ip, "*"))
			continue;

		//conectar con worker
		socket_t sockWorker = conectar_con((char*)&det->ip, (char*)&det->puerto, "WORKER");
		if(sockWorker == -1)
			return false;

		//pedir archivo local
		char buffer[NOMBRE_ARCHIVO_TMP];
		size = serial_string_pack((char*)&buffer, "s", &det->nombre_archivo_local);
		cabecera = protocol_get_header(OP_WRK_Obtener_Archivo_Local, size);
		paquete = protocol_get_packet(cabecera, &buffer);
		if(!protocol_packet_send(sockWorker, &paquete)) {
			socket_close(sockWorker);
			return false;
		}
		//recibir archivo temporal y escribirlo a disco con el nombre temporal que se recuperó de yama
		paquete = protocol_packet_receive(sockWorker);
		if(paquete.header.operation == OP_ERROR)
			return false;
		guardar_archivo_tmp(config->path_tmp, (char*)&det->nombre_archivo_local, paquete.payload, paquete.header.size, es_txt);
		protocol_packet_free(&paquete);

		socket_close(sockWorker);
	}

	//escribir resultado de apareo en base a la lista de nombre de archivos temporales de reduccion
	char nombre_apareo_tmp[NOMBRE_ARCHIVO_TMP];
	global_nombre_aleatorio("a_rg_", &nombre_apareo_tmp, 8);
	char **arc_reduccion_lista = string_split(nombres_arc, TOKEN_SEPARADOR_ARCHIVOS);
	if(!apareo_realizar(arc_reduccion_lista, &nombre_apareo_tmp, config->path_tmp))
		return false;
	free(nombres_arc);
	free(arc_reduccion_lista);

	char *path_apareo = string_from_format("%s%s", config->path_tmp, &nombre_apareo_tmp);
	char *path_script = string_from_format("%s%s", config->path_tmp, &nombre_script_tmp);
	char *path_resultado = string_from_format("%s%s", config->path_tmp, &nombre_archivo_reduccion_global);

	char *cmd = string_new();
	string_append_with_format(&cmd, "cat %s | %s > %s", path_apareo, path_script, path_resultado);
	log_msg_info("operaciones | RG Socket Master [ %d ] Comando a ejecutar [ %s ]", sockMaster, cmd);

	//asignar permisos a script reductor
	chmod(path_script, S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH);

	resultado_t resultado;
	if(system(cmd) == -1) {
		resultado = RESULTADO_Error;
		log_msg_error("operaciones | RG Socket Master [ %d ] No se pudo ejecutar el comando [ %s ]", sockMaster, strerror(errno));
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

bool op_almacenamiento_final(packet_t *packet, socket_t sockMaster, yamaworker_t* config) {
	header_t cabecera;
	packet_t paquete;
	size_t size;

	log_msg_info("operaciones | Etapa Almacenamiento Final: Socket Master [ %d ]", sockMaster);

	//recibir nombre archivo final temporal y nombre yamafs
	char nombre_archivo_tmp[NOMBRE_ARCHIVO_TMP];
	char nombre_archivo_yamafs_tmp[NOMBRE_ARCHIVO_TMP];
	serial_string_unpack(packet->payload, "s s", &nombre_archivo_tmp, &nombre_archivo_yamafs_tmp);
	protocol_packet_free(packet);

	//conectar con filesystem
	socket_t sockFS = conectar_con(config->yamafs_ip, config->yamafs_puerto, "YAMAFS");
	if(sockFS == -1)
		return false;

	//enviar Iniciar Almacenamiento Final
	char buffer[NOMBRE_ARCHIVO_TMP];
	size = serial_string_pack(&buffer, "s", &nombre_archivo_yamafs_tmp);
	cabecera = protocol_get_header(OP_FSY_Almacenar_Archivo, size);
	paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(sockFS, &paquete)) {
		socket_close(sockFS);
		return false;
	}

	//leer archivo
	char *path = string_from_format("%s%s", config->path_tmp, &nombre_archivo_tmp);
	ssize_t size_arc;
	unsigned char *buffer_arc = global_read_txtfile(path, &size_arc);
	cabecera = protocol_get_header(OP_FSY_Almacenar_Archivo, size_arc);
	paquete = protocol_get_packet(cabecera, buffer_arc);
	if(!protocol_packet_send(sockFS, &paquete)) {
		socket_close(sockFS);
		return false;
	}
	free(buffer_arc);

	//recibir respuesta del FS
	paquete = protocol_packet_receive(sockFS);
	if(paquete.header.operation == OP_ERROR)
		return false;
	resultado_t resultado;
	serial_string_unpack(paquete.payload, "h", &resultado);
	protocol_packet_free(&paquete);

	socket_close(sockFS);

	//responder a master
	char buffer2[RESPUESTA_SIZE];
	size = serial_string_pack(&buffer2, "h", resultado);
	cabecera = protocol_get_header(OP_WRK_Iniciar_Reduccion, size);
	paquete = protocol_get_packet(cabecera, &buffer2);
	if(!protocol_packet_send(sockMaster, &paquete))
		return false;

	return true;
}
