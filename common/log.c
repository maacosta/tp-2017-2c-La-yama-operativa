#include "log.h"

#define LOG_BUFFER_SIZE 256

static t_log *logger = NULL;

static void log_template(bool error, const char *format, va_list args) {
	if(!LOG_ENABLED) return;

	if(logger == NULL) {
		log_init("log.txt", "log", LOG_STDOUT);
	}

	char message[LOG_BUFFER_SIZE];

	vsnprintf(message, LOG_BUFFER_SIZE, format, args);

	if(error) {
		log_error(logger, message);
	} else {
		log_debug(logger, message);
	}
}

void log_init(char* file, char *name, bool is_active_console) {
	if(logger == NULL) {
		logger = log_create(
				file,
				name,
				is_active_console,
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
