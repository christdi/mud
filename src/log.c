#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

#include "mud/log.h"

static log_level_t min_log_level = TRACE;

static const char* log_get_level(log_level_t level);

/**
 * Logs a line to stdout if the specified log level is greater than the
 * specified minimum log level.
 *
 * Parameters
 *  level - the level at which to log this line
 *  origin - the function logging the line
 *  format - formatted spring for use with vsprintf to build log line
 *  ... - varargs for vsprintf to build log line
**/
void mlog(log_level_t level, const char *origin, const char* format, ...) {
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

  printf("[%-16s %-5s] %s() - %s\n\r", date_time, log_get_level(level), origin, log_line);
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
  switch(level) {
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
