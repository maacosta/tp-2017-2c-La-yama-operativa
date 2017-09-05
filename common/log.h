#ifndef log_h
#define log_h

#define LOG_ENABLED true
#define LOG_STDOUT true

/**
 * Inicializa el log
 */
void log_init(char* file, char *name);

/**
 * Informa un mensaje de informacion
 * @param format Cadena con formato indicando el mensaje de log.
 */
void log_msg_info(const char *format, ...);

/**
 * Informa un error
 * @param format Cadena con formato indicando el mensaje de log.
 */
void log_msg_error(const char *format, ...);

#endif /* log_h */
