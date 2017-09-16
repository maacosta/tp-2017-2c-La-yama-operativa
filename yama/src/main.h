#ifndef MAIN_YAMA_H_
#define MAIN_YAMA_H_

#include "../../common/log.h"
#include "../../common/socket.h"
#include "../../common/protocol.h"
#include "configuration.h"

typedef struct{
	int job;
	int master;
	char nodo[50];
	int bloque;
	char etapa;					//transformacion, reduccion local, reduccion global, almacenamiento final
	char archivo_temporal[261];	//el path es temp/nombre_aleatorio hasta 255
	char estado;				//en proceso, finalizado ok, error
} estado_master_t;

#endif /* MAIN_YAMA_H_ */
