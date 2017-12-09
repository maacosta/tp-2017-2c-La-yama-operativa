#include "operaciones_local.h"

bool op_obtener_archivo_local(packet_t *packet, socket_t sockWorker, yamaworker_t* config) {
	header_t cabecera;
	packet_t paquete;
	size_t size;

	log_msg_info("operaciones local | Obtencion de archivo: socket worker [ %d ]", sockWorker);

	//recibir informacion de bloque y nombre archivo final temporal
	char nombre_archivo_tmp[NOMBRE_ARCHIVO_TMP];
	serial_string_unpack(packet->payload, "s", &nombre_archivo_tmp);
	protocol_packet_free(packet);

	//enviar archivo local
	char *path = string_from_format("./%s/%s", config->path_tmp, &nombre_archivo_tmp);
	ssize_t size_arc;
	unsigned char *buffer_arc = global_read_txtfile(path, &size_arc);
	cabecera = protocol_get_header(OP_WRK_Obtener_Archivo_Local, size_arc);
	paquete = protocol_get_packet(cabecera, buffer_arc);
	if(!protocol_packet_send(sockWorker, &paquete))
		return false;
	free(buffer_arc);

	return true;
}
