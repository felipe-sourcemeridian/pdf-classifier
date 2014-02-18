#include "file_server.h"
server_info *configure_server(GKeyFile *_config_file,int *_server_status)
{
	int _fd = -1;
	int _flags=0;
	struct sockaddr_in _server_addr;
	server_info *_server_info = NULL;
	int _port=0;
	int _max_request;
	int *_request_poll=NULL;
	double _accept_timeout=0;
	_fd = socket(AF_INET, SOCK_STREAM, 0);
	_port = g_key_file_get_integer(_config_file, SERVER_GROUP_NAME, SERVER_PORT, NULL);
	_max_request = g_key_file_get_integer(_config_file,SERVER_GROUP_NAME,SERVER_MAX_REQUEST,NULL);
	_accept_timeout = g_key_file_get_double(_config_file,SERVER_GROUP_NAME,SERVER_ACCEPT_TIMEOUT,NULL);
	if (_fd < 0)
	{
		*_server_status=SERVER_FD_BAD;
	        return NULL;
	}
	if(_port == 0)
	{
		*_server_status = SERVER_PORT_IS_NOT_SET;	
		return NULL;
	}
	if(_max_request==0)
	{
		*_server_status = SERVER_MAX_REQUEST_IS_NOT_SET;
		return NULL;
	}
	bzero(&_server_addr, sizeof (struct sockaddr_in));
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_port = htons(_port);
	_server_addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(_fd, (struct sockaddr *) &_server_addr, sizeof (_server_addr)) < 0)
	{
		close(_fd);
		*_server_status=SERVER_PORT_BUSY;
		return NULL;
	}
	if (listen(_fd, _max_request) < 0) 
	{
		*_server_status=SERVER_PORT_BUSY;
		close(_fd);
		return NULL;
	}
	_server_info = (server_info *) malloc(sizeof (server_info));
	if (_server_info == NULL)
	{
		*_server_status=SERVER_ALLOC;
        	close(_fd);
	        return NULL;
	}
	_request_poll=(int *)malloc(sizeof(int)*(_max_request/2));
	if(_request_poll==NULL)
	{
		*_server_status=SERVER_ALLOC;
		free(_server_info);
        	close(_fd);
	        return NULL;
	}
	_server_info->fd_server = _fd;
	_server_info->request_poll=_request_poll;
	_server_info->request_poll_current_size=0;
	_server_info->accept_timeout=_accept_timeout;
	_server_info->request_poll_capacity=(_max_request/2);
	_server_info->sockaddr = (struct sockaddr *)&_server_addr;
	_flags= fcntl(_fd, F_GETFL, 0);
	fcntl(_fd, F_SETFL, _flags | O_NONBLOCK);
	*_server_status=SERVER_OK;
	return _server_info;
}
int shutdown_server(server_info *_server_info) {
    close(_server_info->fd_server);
    return 0;
}
void delete_server_info(server_info *_server_info)
{
	free(_server_info);	
}
int fill_new_requests_into_server(server_info *server, int max_request) {
	int capacity = max_request;
	struct sockaddr_in client_addr;
	socklen_t client_length;	
	int current_size = 0;
	int current_index = 
	client_length = sizeof (client_addr);
	int count_new_request = 0;
	int current_index = server->request_poll_current_size;
	int fd = 1 ;
	while(fd > 0)
	{
		fd = accept4(server_info->fd_server, (struct sockaddr *) &client_addr, &client_length, O_NONBLOCK);
		if(fd > 0)
		{
			server->request_poll[_current_size]=fd;
		}
	}
	_server_info->request_poll_current_size=_current_size;
	return ;
}
