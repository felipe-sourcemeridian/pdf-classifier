#include "stdlib.h"
#include <stdio.h>
#include "fileserver.h"
void start_server_and_start_epoll(file_server *server);
void onclose(void *data, file_server *server);
void onread(void *data, file_server *server);
typedef struct
{
	file_server *server;
	int fd;
}client;
int main(int argc, char **argv)
{
	int new_client = 0;
	client *cli;
	file_server *server = (file_server *)malloc(sizeof(file_server));
	server->events_size = 50;
	server->port = 5000;
	server->events = (struct epoll_event *)calloc(server->events_size, sizeof(struct epoll_event));
	start_server_and_start_epoll(server);
	while(1)
	{
		new_client = get_new_request_on_server(server);
		if(new_client > 0)
		{
			printf("new connection fd:%d\n", new_client);
			cli = (client *)malloc(sizeof(client));
			cli->fd = new_client;
			cli->server = server;
			register_client_on_server(new_client, server, cli);
		}
		else if(new_client < 0)
		{
			printf("erro bad fd %s\n", strerror(errno));
		}
		process_events_on_server(server, onread, onclose, 200);
	}
		
}
void onclose(void *data, file_server *server)
{
	client *cli= (client *)data;
	printf("handle close event \n");
	unregister_client_on_server(cli->fd, cli->server);	
	close(cli->fd);
	printf("close fd %d\n", cli->fd);
	free(cli);
}
void onread(void *data, file_server *server)
{	
	char buffer[400];
	client *cli= (client *)data;
	int fd = cli->fd;
	int bytes = read(fd, buffer, 400);
	printf("handle read event %d\n", bytes);
	while(bytes > 0)
	{
		buffer[bytes] = 0;
		printf("fd :%d data:%s\n", fd, buffer);
		bytes = read(fd, buffer,400);
	}
}
void start_server_and_start_epoll(file_server *server)
{
	if(start_server(server, 50) < 0)
	{
		printf("error starting server %s\n",strerror(errno));
		exit(-1);
	}
	if(start_epoll_on_server(server) < 0)
	{
		printf("error starting epoll %s\n",strerror(errno));	
		exit(-1);
	}
}

