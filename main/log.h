#ifndef LOG_H
#define LOG_H

#include <stdint.h>

int log_start(const char *host, uint16_t port);
int log_stop(void);

int log_initialize(void);

#endif
