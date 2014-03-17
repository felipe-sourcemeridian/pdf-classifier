#ifndef CLIENT_WRITER
#define CLIENT_WRITER
#include "errno.h"
#include <syslog.h>
#define IS_TRY_AGAIN_WRITE_ERROR(byte_to_write)	((byte_to_write) < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
#ifdef __cplusplus
extern "C" {
#endif

int write_to_client(int client, void *data, int bytes_to_write);

#ifdef __cplusplus
}
#endif

#endif
