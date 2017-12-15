#ifndef SRC_ESTADOS_MASTER_H_
#define SRC_ESTADOS_MASTER_H_

#include <stdbool.h>
#include <commons/collections/list.h>
#include "../../common/global.h"
#include "../../common/socket.h"
#include "../../common/protocol.h"
#include "log_planificador.h"

void em_inicializar();

/*
 * Obtiene el siguiente numero de job para el ciclo completo de transformacion/reduccion
 */
int em_obtener_proximo_numero_job();
estado_master_t *em_agregar_estado_transformacion(char *nombre_nodo, int num_bloque, socket_t sockMaster);
estado_master_t *em_agregar_estado_reduccion(int num_job, char *nombre_nodo, socket_t sockMaster);
estado_master_t *em_agregar_estado_reduccion_global(int num_job, char *nombre_nodo, socket_t sockMaster);
estado_master_t *em_agregar_estado_almacenamiento_final(int num_job, char *nombre_nodo, socket_t sockMaster);
estado_master_t *em_actualizar_estado_bloque(int num_job, char *nombre_nodo, int num_bloque, resultado_t resultado);
t_list *em_obtener_listado(int num_job, etapa_t etapa);

void em_finalizar();

#endif /* SRC_ESTADOS_MASTER_H_ */
