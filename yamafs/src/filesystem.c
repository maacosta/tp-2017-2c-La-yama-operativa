#include "filesystem.h"

static bool procesar_bloque(int num_copia, bloque_t *b, char *nombre_nodo, int *numero_bloque, yamafs_t *config) {
	//obtener nodo segun rnd entre bloques libres
	int bloques_total;
	int bloques_libres;
	nodo_obtener_rnd(nombre_nodo, &bloques_total, &bloques_libres);
	log_msg_info("filesystem | Copia [ %d ] Nodo designado [ %s ]", num_copia, nombre_nodo);

	//obtener detalle nodo
	nodo_detalle_t *det_nodo = nodos_obtener_datos_nodo(nombre_nodo);
	if(det_nodo == NULL) {
		log_msg_error("filesystem | No se encontro detalle para el nodo [ %s ] de la copia [ %d ]", nombre_nodo, num_copia);
		return false;
	}

	//obtener bloque libre
	bitmap_t bm = bitmap_abrir(config, nombre_nodo, bloques_total);
	int num_bloque;
	for(num_bloque = 0; num_bloque < bloques_total; num_bloque++) {
		if(!bitmap_get(&bm, num_bloque)) break;
	}
	*numero_bloque = num_bloque;
	log_msg_info("filesystem | Copia [ %d ] Bloque designado [ %d ]", num_copia, num_bloque);

	//enviar bloque a datanode
	header_t cabecera;
	packet_t paquete;
	size_t size;
	char buffer[BLOQUE_SIZE_E];
	size = serial_string_pack(&buffer, "h", num_bloque);
	cabecera = protocol_get_header(OP_DND_Almacenar_Bloque, size);
	paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(det_nodo->socket, &paquete))
		return false;

	cabecera = protocol_get_header(OP_DND_Almacenar_Bloque, b->size);
	paquete = protocol_get_packet(cabecera, &b->stream);
	if(!protocol_packet_send(det_nodo->socket, &paquete))
		return false;

	//guardar bloque en el sistema
	//actualizar bloques libres del nodo
	nodo_actualizar(nombre_nodo, bloques_libres - 1);
	//actualizar bitmap de bloques usados
	bitmap_set(&bm, num_bloque);
	bitmap_destruir(&bm);

	return true;
}

static bool filesystem_copiar_a_yamafs(unsigned char *stream, ssize_t size, const char *nom_archivo, int indice, bool es_txt, yamafs_t *config) {
	//obtener bloques de streaming
	t_list *bloques = list_create();
	bloque_t *bloque;
	int len, len_packet = 0;
	if(es_txt) {
		int i, j, k, z;
		i = k = 0;

		for(j = 0; j < size; j++) {
			if(stream[j] == '\n' || j - i >= TAMANIO_BLOQUE - 1 || j == size - 1) {
				if(j - i >= TAMANIO_BLOQUE - 1 || j == size - 1) {
					if(k == i && j != size - 1) z = TAMANIO_BLOQUE;
					else if(j == size - 1) z = size - i;
					else z = k - i + 1;

					bloque = malloc(sizeof(bloque_t));
					strcpy(&bloque->stream, string_substring(stream, i, z));
					bloque->size = z;
					list_add(bloques, bloque);

					i += z;
					k = i;
				}
				if(stream[j] == '\n' && j >= i) k = j;
			}
		}
	}
	else {
		ssize_t s = size;
		do {
			bloque = malloc(sizeof(bloque_t));
			len = s > TAMANIO_BLOQUE ? TAMANIO_BLOQUE : s;
			s -= len;

			memcpy(&bloque->stream, stream + len_packet, len);
			bloque->size = len;

			len_packet += len;

			list_add(bloques, bloque);
		} while(s > 0);
	}

	//validar cantidad de bloques libres
	if(list_size(bloques) * 2 > nodo_obtener_bloques_libres()) {
		log_msg_error("filesystem | Se generaron [ %d ] bloques (duplicados) y solo hay [ %d ] bloques libres", list_size(bloques) * 2, nodo_obtener_bloques_libres());
		list_destroy_and_destroy_elements(bloques, (void *)free);
		return false;
	}

	//crear archivo config
	t_config *arc_config = archivo_cargar(config, nom_archivo, indice);
	archivo_definir_cabecera(arc_config, list_size(bloques), size, es_txt);

	//spreading de bloques por los nodos
	bool hay_error = false;
	int indice_bloque = 0;
	void iterar(bloque_t *b) {
		if(hay_error) return;

		char nombre_nodo_0[NOMBRE_NODO_SIZE];
		char nombre_nodo_1[NOMBRE_NODO_SIZE];
		int num_bloque_0, num_bloque_1;
		if(!procesar_bloque(0, b, &nombre_nodo_0, &num_bloque_0, config)) {
			hay_error = true;
			return;
		}
		if(!procesar_bloque(1, b, &nombre_nodo_1, &num_bloque_1, config)) {
			hay_error = true;
			return;
		}

		//agregar archivo al sistema con detalle de bloques
		archivo_definir_copias(arc_config, indice_bloque, b->size, &nombre_nodo_0, num_bloque_0, &nombre_nodo_1, num_bloque_1);

		indice_bloque++;
	}
	list_iterate(bloques, (void *)iterar);

	archivo_destruir(arc_config);

	if(hay_error) {
		//borrar config de archivo
		archivo_borrar(config, nom_archivo, indice);
	}

	return true;
}

static bool obtener_bloque(int num_bloque, int tamanio_bloque, socket_t sock, unsigned char *bloque) {
	packet_t packet;
	header_t cabecera;
	packet_t paquete;
	size_t size;

	char buffer[BLOQUE_SIZE_E + BYTES_OCUPADOS_SIZE_E + 1];
	size = serial_string_pack(&buffer, "h h", num_bloque, tamanio_bloque);
	cabecera = protocol_get_header(OP_DND_Obtener_Bloque, size);
	paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(sock, &paquete))
		return false;
	packet = protocol_packet_receive(sock);
	if(packet.header.operation == OP_ERROR) {
		socket_close(sock);
		return false;
	}
	memcpy(bloque, packet.payload, tamanio_bloque);
	protocol_packet_free(&packet);
	return true;
}

bool filesystem_cpfrom(const char *path_origen, const char *nom_archivo, int indice, bool es_txt, yamafs_t *config) {
	//obtener streaming de datos
	unsigned char *stream;
	ssize_t size;
	if(es_txt) stream = global_read_txtfile(path_origen, &size);
	else stream = global_read_binfile(path_origen, &size);

	return filesystem_copiar_a_yamafs(stream, size, nom_archivo, indice, es_txt, config);
}

bool filesystem_almacenamiento_final(packet_t *packet, socket_t sockWorker, yamafs_t *config) {
	header_t cabecera;
	packet_t paquete;
	size_t size;

	log_msg_info("filesystem | Almacenar archivo final: socket [ %d ]", sockWorker);

	//recibir nombre archivo final temporal y nombre yamafs
	char nombre_archivo_yamafs_tmp[NOMBRE_ARCHIVO_TMP];
	serial_string_unpack(packet->payload, "s", &nombre_archivo_yamafs_tmp);
	protocol_packet_free(packet);

	//recibir archivo temporal y escribirlo a disco con un nombre temporal
	paquete = protocol_packet_receive(sockWorker);
	if(paquete.header.operation == OP_ERROR) {
		return false;
	}
	char archivo[NOMBRE_ARCHIVO];
	int indice = directorio_obtener_indice(&nombre_archivo_yamafs_tmp, &archivo);
	bool r = filesystem_copiar_a_yamafs(paquete.payload, paquete.header.size, &archivo, indice, true, config);
	protocol_packet_free(&paquete);

	//responder a worker
	char buffer[RESPUESTA_SIZE];
	size = serial_string_pack(&buffer, "h", (r ? RESULTADO_OK : RESULTADO_Error));
	cabecera = protocol_get_header(OP_FSY_Almacenar_Archivo, size);
	paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(sockWorker, &paquete)) {
		return false;
	}

	return true;
}

bool filesystem_cpto(const char *path_destino, const char *nom_archivo, int indice, yamafs_t *config) {
	t_config *arc_config = archivo_cargar(config, nom_archivo, indice);
	int cant_bloques;
	int tamanio;
	bool es_txt;
	archivo_recuperar_cabecera(arc_config, &cant_bloques, &tamanio, &es_txt);

	unsigned char *stream = malloc(tamanio);

	nodo_detalle_t *det_nodo;
	int i, j = 0;
	int bytes, bloque_0, bloque_1;
	char nombre_nodo_0[NOMBRE_NODO_SIZE], nombre_nodo_1[NOMBRE_NODO_SIZE];
	unsigned char bloque[BLOQUE_LEN];
	bool hay_error = false;
	for(i = 0; i < cant_bloques; i++) {
		archivo_recuperar_copias(arc_config, i, &bytes, &nombre_nodo_0, &bloque_0, &nombre_nodo_1, &bloque_1);

		//intenta recuperar con copia bloque 0
		det_nodo = nodos_obtener_datos_nodo(&nombre_nodo_0);
		if(obtener_bloque(bloque_0, bytes, det_nodo->socket, &bloque)) {
			memcpy(stream + j, &bloque, bytes);
			j += bytes;
			continue;
		}
		log_msg_error("filesystem | Fallo copia [ 0 ] del nodo [ %s ]", nombre_nodo_0);

		//intenta recuperar con copia bloque 0
		det_nodo = nodos_obtener_datos_nodo(&nombre_nodo_1);
		if(obtener_bloque(bloque_1, bytes, det_nodo->socket, &bloque)) {
			memcpy(stream + j, &bloque, bytes);
			j += bytes;
			continue;
		}
		log_msg_error("filesystem | Fallo copia [ 1 ] del nodo [ %s ]", nombre_nodo_1);

		//ambas copias fallaron
		hay_error = true;
		break;
	}

	if(!hay_error) {
		log_msg_info("filesystem | Creacion de archivo [ %s ]", path_destino);
		if(es_txt) global_create_txtfile(path_destino, stream, tamanio);
		else global_create_binfile(path_destino, stream, tamanio);
	}

	free(stream);
	archivo_destruir(arc_config);

	return true;
}
