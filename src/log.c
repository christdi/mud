#include "mud/log.h"

extern zlog_category_t * nc;

int log_initialise(const char * configFilePath) {
	if (zlog_init(configFilePath) != 0 ) {
		return -1;
	}

	if ((gc = zlog_get_category("general")) == NULL) {
		return -1;
	}

	if ((nc = zlog_get_category("network")) == NULL) {
		return -1;
	}

	if ((dc = zlog_get_category("data")) == NULL) {
		return -1;
	}

	return 0;
}

void log_shutdown(void) {
	zlog_fini();
}