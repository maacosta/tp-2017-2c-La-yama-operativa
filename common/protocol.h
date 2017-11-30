#ifndef COMMON_PROTOCOL_H_
#define COMMON_PROTOCOL_H_

#include "log.h"
#include "global.h"
#include "serial.h"
#include "serial_string.h"
#include "socket.h"

#define BLOQUE_LEN 1048576 //1024*1024 = 1 mb
#define NUMERO_JOB_SIZE 3
#define RESPUESTA_SIZE 2
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

	OP_YAM_Enviar_Estado,
	OP_YAM_Solicitar_Transformacion,
	OP_YAM_Replanificar_Transformacion,
	OP_YAM_Solicitar_Reduccion,
	OP_YAM_Solicitar_Reduccion_Global,
	OP_YAM_Solicitar_Almacenamiento_Final,

	OP_WRK_Iniciar_Transformacion,
	OP_WRK_Iniciar_Reduccion,
	OP_WRK_Iniciar_Reduccion_Global,
	OP_WRK_Iniciar_Almacenamiento_Final,

	OP_FSY_Informacion_Archivo,
	OP_FSY_Almacenar_Archivo,
	OP_FSY_Obtener_Nodos,
	OP_FSY_Registrar_Nodo,

	OP_DND_Obtener_Bloque,
	OP_DND_Almacenar_Bloque,
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

typedef enum {
	ETAPA_Transformacion,
	ETAPA_Reduccion_Local,
	ETAPA_Reduccion_Global,
	ETAPA_Almacenamiento_Final
} etapa_t;

typedef enum {
	ESTADO_En_Proceso,
	ESTADO_Error_Replanifica,
	ESTADO_Finalizado_OK,
	ESTADO_Error,
} estado_t;

typedef enum {
	RESULTADO_Error,
	RESULTADO_OK,
} resultado_t;

typedef struct {
	char nodo[NOMBRE_NODO_SIZE];
	char ip[IP_SIZE];
	char puerto[PUERTO_SIZE];
	unsigned int wl;
	unsigned int executed_jobs;
} detalle_nodo_t;

typedef struct {
	int job;
	socket_t master;
	char nodo[NOMBRE_NODO_SIZE];
	int bloque;
	etapa_t etapa;
	char archivo_temporal[NOMBRE_ARCHIVO_TMP];
	estado_t estado;
} estado_master_t;

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
