#include "worker_test.h"

void worker_enviar_transformacion(socket_t sockWorker) {
	header_t cabecera;
	packet_t paquete;
	size_t size;

	//enviar Iniciar Transformacion
	char buffer[1024];
	size = serial_string_pack(&buffer, "h h s h", 2, 1048564, "arc_res_transformador.txt", true);
	cabecera = protocol_get_header(OP_WRK_Iniciar_Transformacion, size);
	paquete = protocol_get_packet(cabecera, &buffer);
	if(!protocol_packet_send(sockWorker, &paquete))
		return;

	//enviar programa Transformacion
	ssize_t size_arc;
	unsigned char *buffer_arc;
	buffer_arc = global_read_txtfile("./transformador.py", &size_arc);
	cabecera = protocol_get_header(OP_WRK_Iniciar_Transformacion, size_arc);
	paquete = protocol_get_packet(cabecera, buffer_arc);
	if(!protocol_packet_send(sockWorker, &paquete))
		return;
	free(buffer_arc);

	//recibir Iniciar Transformacion
	paquete = protocol_packet_receive(sockWorker);
	if(paquete.header.operation == OP_ERROR)
		return;
	resultado_t resultado;
	serial_string_unpack(paquete.payload, "h", &resultado);
	protocol_packet_free(&paquete);

	printf("Transformacion enviada y procesada con exito");
}
