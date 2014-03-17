#ifndef CLIENT_READER
#define CLIENT_READER
#define IS_TRY_AGAIN_READ_ERROR(byte_read)	((byte_read) < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
#include <sys/time.h>
#include "errno.h"

#ifdef __cplusplus
extern "C" {
#endif

int read_from_client(int client, void *buffer, int byte_to_read, int timeout);

#ifdef __cplusplus
}
#endif

#endif
