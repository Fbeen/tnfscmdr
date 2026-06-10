#include <stdio.h>
#include <stdarg.h>
#include "log.h"

FILE *g_log = NULL;

void log_open(const char *filename)
{
    g_log = fopen(filename, "w");
}

void log_close(void)
{
    if (g_log) {
        fclose(g_log);
        g_log = NULL;
    }
}

void log_printf(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    if (g_log) {
        va_start(args, fmt);
        vfprintf(g_log, fmt, args);
        va_end(args);
        fflush(g_log);
    }
}
