#ifndef YAMAWORKER_MAIN_H_CLIENTE
#define YAMAWORKER_MAIN_H_CLIENTE

#include <stdio.h>
#include <stdlib.h>
#include "../../common/socket.h"
#include "../../common/protocol.h"
#include "configuration.h"
#include <string.h>

#define PUERTO_WORKER 5005;
#define IP_WORK 127.0.0.1;
/*
#OP >> WRK Iniciar_Transformacion
        <archivo_transformacion>
        <descripcion_bloque>
            <bloque>
            <bytes_ocupados>
            <nombre_archivo_temporal>
*/



socket_t conectar_con_worker_prueba();

#endif /* YAMAWORKER_MAIN_H_CLIENTE */
