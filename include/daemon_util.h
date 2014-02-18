#ifndef DAEMON_UTIL_H
#define DAEMON_UTIL_H
#include <sys/types.h>
#include <unistd.h>
#include "syslog.h"
#include "stdio.h"
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
extern const char *LOG_NAME;
void init_daemon(const char *_current_directory);
#endif
