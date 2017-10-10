#ifndef COMMON_SERIAL_STRING_H_
#define COMMON_SERIAL_STRING_H_

#include <string.h>
#include <commons/string.h>

size_t serial_string_pack(char *buf, char *format, ...);

void serial_string_unpack(char *buf, char *format, ...);

#endif /* COMMON_SERIAL_STRING_H_ */
