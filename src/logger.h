#ifndef good_LOGGER_H
#define good_LOGGER_H

#include "error.h"
#include <stdio.h>

#define good_LOGLEVEL_INFO  "INFO "
#define good_LOGLEVEL_ERROR "ERROR"

int good_initialize_logger(const char *log_file_path);
void good_finalize_logger();
void good_log(const char *level, const char *file_path, size_t line, const char *format, ...);
void good_log_array(const char *array[], size_t len);

#define good_log_info(format) good_log(good_LOGLEVEL_INFO, __FILE__, __LINE__, format)
#define good_log_error(format) good_log(good_LOGLEVEL_ERROR, __FILE__, __LINE__, format)

#define good_log_infof(format, ...) good_log(good_LOGLEVEL_INFO, __FILE__, __LINE__, format, __VA_ARGS__)
#define good_log_errorf(format, ...) good_log(good_LOGLEVEL_ERROR, __FILE__, __LINE__, format, __VA_ARGS__)

#endif
