#include "request_manager_builder.h"
static file_server *create_server(int port, int max_request);

request_manager *create_request_manager(int port, int timeout, int max_request, classifier_request_list *requests, REQUEST_CALLBACK onrequest_event, REQUEST_ERROR_CALLBACK onrequest_error, REQUEST_WRITE_RESPONSE_CALLBACK onwrite_response)
{
	file_server *server = create_server(port, max_request);
	request_manager *manager = NULL;
	if(server == NULL)
	{
		return NULL;
	}
	manager = (request_manager *) malloc(sizeof(request_manager));
	if(manager == NULL)
	{
		free(server->events);
		free(server);
		return NULL;
	}
	server->handler_data = manager;
	manager->server = server;
	manager->request_timeout = timeout;
	manager->requests = requests;
	manager->onrequest_event = onrequest_event;
	manager->onrequest_error = onrequest_error;
	manager->onwrite_reponse_event = onwrite_response;
	return manager;	
}

static file_server *create_server(int port, int max_request)
{
	file_server *server = (file_server *)malloc(sizeof(file_server));
	if(server == NULL)
	{
		return server;
	}
	server->events_size = max_request;
	server->port = port;	
	server->events = (struct epoll_event *)calloc(server->events_size, sizeof(struct epoll_event));
	if(server->events == NULL)	
	{
		free(server);
	}
	return server;
}
