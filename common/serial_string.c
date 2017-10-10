#include "serial_string.h"

size_t serial_string_pack(char *buf, char *format, ...) {
	va_list ap;
	va_start(ap, format);

	char* b = string_new();
	char* s;
	int *h;
	char** ssf = string_split(format, " ");
	int i = 0, j = 0;
	for(; ssf[j] != NULL; j++) ;
	for(; i < j; i++) {
		switch(ssf[i][0]) {
		case 's':
			s = va_arg(ap, char*);
			string_append(&b, s);
			break;
		case 'h':
			h = va_arg(ap, int*);
			string_append(&b, string_itoa(h));
		}
		if(i + 1 < j) string_append(&b, "|");
	}
	size_t len = string_length(b);
	memcpy(buf, b, len + 1);
	free(b);
	return len + 1;
}

void serial_string_unpack(char *buf, char *format, ...) {
	va_list ap;
	va_start(ap, format);

	char* s;
	int *h;
	char** ss = string_split((char *)buf, "|");
	char** ssf = string_split(format, " ");
	int i = 0;
	for(; ssf[i] != NULL; i++) {
		switch(ssf[i][0]) {
		case 's':
			s = va_arg(ap, char*);
			int len = string_length(ss[i]);
			memcpy(s, ss[i], len + 1);
			break;
		case 'h':
			h = va_arg(ap, int*);
			*h = atoi(ss[i]);
			break;
		}
		free(ss[i]);
		free(ssf[i]);
	}
	free(ss);
	free(ssf);
}
