#ifndef FILESERVER
#define FILESERVER
#include "fcntl.h"
#include <sys/types.h>
#include <sys/socket.h>
#include "sys/epoll.h"
#include <netinet/in.h>
#include <unistd.h>
#include "errno.h"
#include <fcntl.h>
#include "stdlib.h"
#include "string.h"
#include <stdio.h>
#define EVENTS_TO_WATCH  EPOLLERR |EPOLLRDHUP | EPOLLHUP | EPOLLET | EPOLLIN | EPOLLOUT
#define IS_CLOSE_EVENT(events)	((events & EPOLLRDHUP) || (events & EPOLLERR) || (events & EPOLLHUP))
#define IS_READY_READ_EVENT(events)	((events & EPOLLIN))
#define IS_WRITE_EVENT(events)	(events & EPOLLOUT)
typedef struct
{
	int fd;
	int port;
	int epoll_fd;
	struct epoll_event *events;
	int events_size;
	void *handler_data; 
}file_server;
typedef void (*EVENT_CALLBACK)(void *data, file_server *server);
int start_server(file_server *server, int active_requests);

int start_epoll_on_server(file_server *server);

int get_new_request_on_server(file_server *server);

int register_client_on_server(int client, file_server *server, void *data);

int unregister_client_on_server(int client, file_server *server);

int process_events_on_server(file_server *server, EVENT_CALLBACK read_callback, EVENT_CALLBACK close_callback, EVENT_CALLBACK write_callback, int timeout);

#endif
