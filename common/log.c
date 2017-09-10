#include "log.h"
#include <commons/string.h>
#include <commons/log.h>

#define LOG_BUFFER_SIZE 256

static t_log *logger = NULL;

static void log_template(bool error, const char *format, va_list args) {
	if(!LOG_ENABLED) return;

	char message[LOG_BUFFER_SIZE];

	vsnprintf(message, LOG_BUFFER_SIZE, format, args);

	if(error) {
		log_error(logger, message);
	} else {
		log_debug(logger, message);
	}
}

void log_init(char* file, char *name) {
	if(logger == NULL) {
		logger = log_create(
				file,
				name,
				LOG_STDOUT,
				LOG_LEVEL_TRACE);
	}

}

void log_msg_info(const char *format, ...) {
	va_list args;
	va_start(args, format);
	log_template(false, format, args);
	va_end(args);
}

void log_msg_error(const char *format, ...) {
	va_list args;
	va_start(args, format);
	log_template(true, format, args);
	va_end(args);
}
