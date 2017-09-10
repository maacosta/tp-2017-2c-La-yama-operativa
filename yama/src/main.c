#include "main.h"
#include "../../common/log.h"


int main(int argc, char **argv) {
	log_init("log", "yama", true);
	log_msg_error("esto es un error");
	return 0;
}
