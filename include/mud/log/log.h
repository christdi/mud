#ifndef HG_LOG_H
#define HG_LOG_H

#include <zlog.h>

zlog_category_t * gc;
zlog_category_t * nc;
zlog_category_t * dc;

int log_initialise(const char * configFilePath);
void log_shutdown(void);

#endif