#include "fileserver.h"
int start_server(file_server *server, int active_requests)
{
	int fd = -1;
	int flags=0;
	struct sockaddr_in server_addr;
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
	{
		return -1;
	}
	bzero(&server_addr, sizeof (struct sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(server->port);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(fd, (struct sockaddr *) &server_addr, sizeof (server_addr)) < 0) 
	{
		close(fd);
		return -1;
	}
	if (listen(fd, active_requests) < 0) 
	{
		close(fd);
		return -1;
	}
	server->fd = fd;	
	flags= fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	return 0;
}

int start_epoll_on_server(file_server *server)
{
	int epoll_fd = epoll_create(server->events_size);
	if(epoll_fd < 0)
	{
		return -1;
	}
	server->epoll_fd = epoll_fd;
	return 0;
}

int get_new_request_on_server(file_server *server)
{
	struct sockaddr_in client_addr;
	socklen_t client_length = sizeof (client_addr);
	int fd = accept4(server->fd, (struct sockaddr *) &client_addr, &client_length, O_NONBLOCK);
	if(fd > 0)
	{
		return fd;
	}
	else if(errno != EAGAIN && errno != EWOULDBLOCK)
	{
		return -1;
	}
	return 0;
}

int register_client_on_server(int client,file_server  *server, void *data)
{
	struct epoll_event new_client_watcher;
	new_client_watcher.data.ptr = data;
	new_client_watcher.events = EVENTS_TO_WATCH;
	return epoll_ctl(server->epoll_fd, EPOLL_CTL_ADD, client, &new_client_watcher);
}

int unregister_client_on_server(int client, file_server *server)
{
	struct epoll_event dummy_watcher;
	return epoll_ctl(server->epoll_fd, EPOLL_CTL_DEL, client, &dummy_watcher);
}

int process_events_on_server(file_server *server, EVENT_CALLBACK read_callback, EVENT_CALLBACK close_callback, EVENT_CALLBACK write_callback, int timeout)
{
	struct epoll_event *events = server->events;
	int i;
	int event_size = epoll_wait(server->epoll_fd, server->events, server->events_size, timeout);
	uint32_t events_flags = 0;
	if(event_size < -1)
	{
		return -1;
	}
	for(i=0; i < event_size; i++)
	{
		events_flags =events[i].events;
		printf("is close event read hup %d\n",(events_flags & EPOLLRDHUP));
		printf("is clsose event error %d\n",(events_flags & EPOLLERR));
		printf("is clsoe event socket peer close %d\n",(events_flags & EPOLLHUP)); 
		printf("is read event %d\n", events_flags & EPOLLIN);
		printf("is write %d\n", events_flags & EPOLLOUT);

		if(IS_CLOSE_EVENT(events_flags))
		{
			close_callback(events[i].data.ptr, server);
		}
		else if(IS_READY_READ_EVENT(events_flags))
		{
			read_callback(events[i].data.ptr, server);
		}
		else
		{
			write_callback(events[i].data.ptr, server);
		}
	}
	return 0;
}
