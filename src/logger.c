#include "logger.h"
#include <stdarg.h>

static struct {
    int has_initialized;
    FILE *file;
} logger = {0, NULL};

int good_initialize_logger(const char *log_file_path)
{
    FILE *f;

    f = fopen(log_file_path, "w");
    if (f == NULL) {
        return 1;
    }

    logger.has_initialized = 1;
    logger.file = f;

    return 0;
}

void good_finalize_logger()
{
    if (!logger.has_initialized) {
        return;
    }
    
    fclose(logger.file);
    logger.file = NULL;
    logger.has_initialized = 0;
}

void good_log(const char *level, const char *file_path, size_t line, const char *format, ...)
{
    va_list va;

    if (!logger.has_initialized) {
        return;
    }

    fprintf(logger.file, "[%s] %s(%lu) ", level, file_path, line);

    va_start(va, format);
    vfprintf(logger.file, format, va);
    va_end(va);

    fprintf(logger.file, "\n");
}

void good_log_array(const char *array[], size_t len)
{
    size_t i;

    if (!logger.has_initialized) {
        return;
    }

    fprintf(logger.file, "[%s] ", good_LOGLEVEL_INFO);
    for (i = 0; i < len; i++) {
        fprintf(logger.file, "%s", array[i]);
    }
    fprintf(logger.file, "\n");
}
