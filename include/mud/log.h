#ifndef _LOG_H_
#define _LOG_H_


#include <zlog.h>


/**
 * Globals >.<
**/
zlog_category_t * gc;
zlog_category_t * nc;
zlog_category_t * dc;


/**
 * Function prototypes
**/
int log_initialise(const char * configFilePath);
void log_shutdown(void);


#endif