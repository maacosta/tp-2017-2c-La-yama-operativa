#ifndef log_planificador_h
#define log_planificador_h

#define LOG_ENABLED true
#define LOG_STDOUT true

#include <commons/string.h>
#include <commons/log.h>
#include <stdbool.h>

/**
 * Inicializa el log
 */
void log_planificador_init(char* file, char *name, bool is_active_console);

/**
 * Informa un mensaje de informacion
 * @param format Cadena con formato indicando el mensaje de log.
 */
void log_planificador_msg_info(const char *format, ...);

/**
 * Informa un error
 * @param format Cadena con formato indicando el mensaje de log.
 */
void log_planificador_msg_error(const char *format, ...);

#endif /* log_planificador_h */
