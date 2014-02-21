#include "stdlib.h"
#include <stdio.h>
#include "fileserver.h"
#define HEADER_RESPONSE "node id,document frequency,document coverage,summary frequency,summary coverage,fc,fc^15,fc^20,binary score,roc\n"
typedef struct
{
	file_server *server;
	int fd;
	int bytes_writes;
}client;

void start_server_and_start_epoll(file_server *server);

void onclose(void *data, file_server *server);

void onread(void *data, file_server *server);

void onwrite(void *data, file_server *server);

void close_client_connection(client *cli);

int fill_write_buffer(client *cli);

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
		process_events_on_server(server, onread, onclose, onwrite, 200);
	}
		
}
void onclose(void *data, file_server *server)
{
	client *cli= (client *)data;
	printf("handle close event \n");
	close_client_connection(cli);

}



void onread(void *data, file_server *server)
{	
	char buffer[400];
	client *cli= (client *)data;
	int fd = cli->fd;
	int bytes = read(fd, buffer, 400);
	if(bytes < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
	{
		close_client_connection(cli);
		printf("system error close connection :%s \n", strerror(errno));
	}
	
	printf("handle read event: %d fd:%d\n", bytes, cli->fd);
	if(fill_write_buffer(cli) < 0)
	{
		close_client_connection(cli);
		printf("system error close connection :%s  fd:%d \n", strerror(errno), cli->fd);
	}
	while(bytes > 0)
	{
		buffer[bytes] = 0;
		printf("fd :%d data:%s\n", fd, buffer);
		bytes = read(fd, buffer,400);
		if(bytes > 0)
		{
			continue;
		}
		if(errno == EAGAIN || errno == EWOULDBLOCK)
		{
			break;
		}
		else
		{
			close_client_connection(cli);
			printf("system error close connection :%s \n", strerror(errno));	
		}		
	}
}

void onwrite(void *data, file_server *server)
{
	char buffer[400];
	client *cli= (client *)data;
	printf("handler write events fd:%d\n", cli->fd);
	if(fill_write_buffer(cli) < 0)
	{
		printf("system error close on write event connection :%s \n", strerror(errno));
		close_client_connection(cli);

	}
}

int fill_write_buffer(client *cli)
{
	int length = strlen(HEADER_RESPONSE);
	int byteswritten = 0;
	printf("fill write buffer for fd:%d write dummy data\n", cli->fd);
	byteswritten = write(cli->fd, HEADER_RESPONSE, length);
	while(byteswritten == length)
	{
		byteswritten = write(cli->fd, HEADER_RESPONSE, length);
		if(byteswritten > 0)
		{
			continue;
		}
		
		if(errno == EAGAIN || errno == EWOULDBLOCK)
		{
			break;
		}
		else
		{
			return -1;
		}
		
	}
	printf("the write buffer is full now when user read make room for write the server must handler this event \n");
	return 0;

}

void close_client_connection(client *cli)
{
	unregister_client_on_server(cli->fd, cli->server);	
	close(cli->fd);
	printf("close fd %d\n", cli->fd);
	free(cli);
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

