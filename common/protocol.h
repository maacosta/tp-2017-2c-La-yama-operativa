#ifndef COMMON_PROTOCOL_H_
#define COMMON_PROTOCOL_H_

#include "log.h"
#include "global.h"
#include "serial.h"
#include "serial_string.h"
#include "socket.h"

#define RESPUESTA_SIZE 6
#define NOMBRE_NODO_SIZE 20
#define IP_SIZE 16
#define PUERTO_SIZE 7
#define BLOQUE_SIZE_E 3
#define BYTES_OCUPADOS_SIZE_E 10
#define NOMBRE_ARCHIVO_TMP 50
#define NOMBRE_ARCHIVO 50

/* Operaciones del protocolo
 * Las respuestas viajan con el mismo codigo de operacion
 * el contexto y el payload si lo hubieran determina el flujo de ejecucion
 */
typedef enum {
	//General
	OP_ERROR = 0,		//Se utiliza para responder cualquier error, el detalle queda en el emisor
	OP_HANDSHAKE,

	OP_YAM_Solicitar_Transformacion,
	OP_YAM_Estado_Transformacion,
	OP_YAM_Solicitar_Reduccion,
	OP_YAM_Estado_Reduccion,
	OP_YAM_Solicitar_Reduccion_Global,
	OP_YAM_Estado_Reduccion_Global,
	OP_YAM_Solicitar_Almacenamiento_Final,
	OP_YAM_Estado_Almacenamiento_Final,

	OP_WRK_Iniciar_Transformacion,
	OP_WRK_Iniciar_Reduccion,
	OP_WRK_Iniciar_Reduccion_Global,
	OP_WRK_Iniciar_Almacenamiento_Final,

	OP_FSY_Informacion_Archivo,
	OP_FSY_Almacenar_Archivo,
} operation_t;

typedef struct {
	unsigned int process;
	unsigned int operation;
	unsigned long size;
} header_t;

typedef struct {
	header_t header;
	unsigned char *payload;
} packet_t;

header_t protocol_get_header(unsigned int operation, unsigned long size);

packet_t protocol_get_packet(header_t header, unsigned char* buffer);

bool protocol_handshake_send(socket_t sockfd);

bool protocol_handshake_receive(socket_t sockfd, header_t *header);

/**
 * Recibe un paquete del socket. Tener en cuenta de liberar el payload de ser necesario
 */
packet_t protocol_packet_receive(socket_t sockfd);

bool protocol_packet_send(socket_t sockfd, packet_t *packet);

void protocol_packet_free(packet_t *packet);

#endif /* COMMON_PROTOCOL_H_ */
