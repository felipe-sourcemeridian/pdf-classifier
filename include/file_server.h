#ifndef FILE_SERVER
#define FILE_SERVER
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "errno.h"
#include <fcntl.h>
#include "stdlib.h"
#include "string.h"
#include <glib.h>
#include <stdio.h>
#include "config_util.h"
#include "time.h"
#define SERVER_PORT_BUSY	1
#define SERVER_ALLOC	2
#define SERVER_FD_BAD	4
#define SERVER_QUEUE_ERROR 8
#define SERVER_FD_CLOSED -1
#define SERVER_REQUEST_AGAIN 16
#define SERVER_SHUTDOWN	32
#define SERVER_OK	64
#define SERVER_PORT_IS_NOT_SET	128
#define SERVER_MAX_REQUEST_IS_NOT_SET	256
#define SERVER_ACCEPT_TIMEOUT_IS_NOT_SET	512
#define CONVERT_TITLE 1
#define CONVERT_DOCUMENT 2
#define CONVERT_END_DOCUMENT 3
#define PACKET_HEADER_SIZE	8
typedef struct {
	int fd_server;
	int max_request;
	int active_conecctions;
	struct sockaddr *sockaddr;
}server_info;

typedef struct
{
	int32_t size;
	unsigned char type;
	char padding[3];
	char data[0];
}convert_request;

server_info *configure_server(GKeyFile *_config_file,int *_server_status);

int shutdown_server(server_info *_server_info);

int fill_new_request_into_server(server_info *server);

void delete_server_info(server_info *_server_info);
#endif
