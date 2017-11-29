#include "filesystem.h"

static bool procesar_bloque(int num_copia, bloque_t *b, char *nombre_nodo, int *numero_bloque, yamafs_t *config) {
	//obtener nodo segun rnd entre bloques libres
	int bloques_total;
	int bloques_libres;
	nodo_obtener_rnd(nombre_nodo, &bloques_total, &bloques_libres);

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
}

bool filesystem_cpfrom(const char *path_origen, const char *nom_archivo, int indice, bool es_txt, yamafs_t *config) {
	//obtener streaming de datos
	unsigned char *stream;
	ssize_t size;
	if(es_txt) stream = global_read_txtfile(path_origen, &size);
	else stream = global_read_binfile(path_origen, &size);

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
		do {
			bloque = malloc(sizeof(bloque_t));
			len = size > TAMANIO_BLOQUE ? TAMANIO_BLOQUE : size;
			size -= len;

			memcpy(&bloque->stream, stream + len_packet, len);
			bloque->size = len;

			len_packet += len;

			list_add(bloques, bloque);
		} while(size > 0);
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
		procesar_bloque(0, b, &nombre_nodo_0, &num_bloque_0, config);
		procesar_bloque(1, b, &nombre_nodo_1, &num_bloque_1, config);

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
