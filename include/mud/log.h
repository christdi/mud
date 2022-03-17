#ifndef MUD_LOG_H
#define MUD_LOG_H

/**
 * Defines
 **/
#define MAX_LOG_LINE_LENGTH 1024
#define MAX_LOG_DATE_TIME_LENGTH 30

#define LOG(level, line, ...) mlog(level, __FILE__, __LINE__, line, ##__VA_ARGS__)

/**
 * Enumes
 **/
typedef enum log_level {
  TRACE,
  DEBUG,
  INFO,
  WARN,
  ERROR
} log_level_t;

/**
 * Function prototypes
 **/
void mlog(log_level_t level, const char* function, const int line, const char* format, ...);

#endif