#ifndef LOG_H
#define LOG_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

extern FILE *g_log;

void log_open(const char *filename);
void log_close(void);
void log_printf(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* LOG_H */
