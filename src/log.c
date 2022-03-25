#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "mud/log.h"

static log_level_t min_log_level = DEBUG;

static const char* log_get_level(log_level_t level);

/**
 * Logs a line to stdout if the specified log level is greater than the
 * specified minimum log level.
 *
 * Parameters
 *  level - the level at which to log this line
 *  function - the function log is called from
 *  line - the line log is called from
 *  format - formatted spring for use with vsprintf to build log line
 *  ... - varargs for vsprintf to build log line
 **/
void mlog(log_level_t level, const char* function, const int line, const char* format, ...) {
  if (level < min_log_level) {
    return;
  }

  char log_line[MAX_LOG_LINE_LENGTH];

  va_list args;
  va_start(args, format);

  if ((vsnprintf(log_line, MAX_LOG_LINE_LENGTH, format, args)) >= MAX_LOG_LINE_LENGTH) {
    fprintf(stderr, "mlog(): formatted output for log line was too long and was truncated");
  }

  va_end(args);

  char date_time[MAX_LOG_DATE_TIME_LENGTH];
  time_t current_time;
  time(&current_time);
  struct tm* tm = localtime(&current_time);
  strftime(date_time, MAX_LOG_DATE_TIME_LENGTH, "%d-%m-%Y %X", tm);

  printf("%-16s %5s [%s:%d] %s\n\r", date_time, log_get_level(level), function, line, log_line);
}

void hlog(log_level_t level, const char* function, const int line, const char* input, size_t len) {
  if (len == 0) {
    return;
  }

  char hex[len * 3 + 1];

  size_t i = 0;
  size_t j = 0;

  for (i = 0; i < len; i++) {
    sprintf(hex + j, "%02X ", (unsigned char)input[i]);

    j += 3;
  }

  hex[j - 1] = '\0';

  mlog(level, function, line, "%s", hex);  
}

/**
 * Set the minimum log level below which logs will not be printed.
 *
 * Parameters
 *  level - the minimum log level to be set
 **/
void log_set_level(const log_level_t level) {
  min_log_level = level;
}

/**
 * Returns a represensation of the provided log level.
 *
 * Parameters
 *  level - the log level to return a string of
 **/
static const char* log_get_level(log_level_t level) {
  switch (level) {
  case ERROR:
    return "ERROR";
  case WARN:
    return "WARN";
  case INFO:
    return "INFO";
  case DEBUG:
    return "DEBUG";
  case TRACE:
    return "TRACE";
  }

  return "APOCALYPTIC";
}
