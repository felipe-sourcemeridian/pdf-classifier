#include "request_manager.h"
static void onread(void *data, file_server *server);

static void onclose(void *data, file_server *server);

static void onwrite(void *data, file_server *server);

static void find_and_notify_all_request_timeout(request_manager *request_manafer);

static int read_request_data(classifier_request *request);

static void process_request_header(classifier_request *request, request_manager *request_manager);

static void process_request(classifier_request *request, request_manager *request_manager);

static classifier_request *get_request_not_active_from_request_list(classifier_request_list *requests);



int start_server_and_register_event_handler(request_manager *request_manager, int max_request)
{
	if(start_server(request_manager->server, max_request) < 0)
	{
		return -1;
	}

	if(start_epoll_on_server(request_manager->server) < 0)
	{
		return -1;
	}
	return 0;
}


int close_request_on_request_manager(classifier_request *request,request_manager *request_manager)
{
	if(unregister_client_on_server(request->fd, request_manager->server) < 0) 
	{
		printf("error unregister client fd:%d\n", request->fd);
		return -1;
	}
	if(close(request->fd) < 0)
	{
		printf("error close connection %d\n", request->fd);
		return -1;
	}
	reset_classifier_request_to_default_state(request);
	request_manager->requests->current_size -= 1; 
	return 0;	
}

int add_news_request_to_request_manager(request_manager *request_manager)
{	
	int fd = 1;
	classifier_request_list *requests = request_manager->requests;
	classifier_request *request = NULL;
	file_server *server = request_manager->server;
	struct timeval start_request_time;
	while(CAN_HANDLE_MORE_REQUEST(requests) && fd > 0)
	{
		fd = get_new_request_on_server(server);	
		if(fd < -1)
		{
			return -1;
		}
		if(fd == 0)
		{
			return 0;
		}
		printf("new connection %d\n", fd);
		request = get_request_not_active_from_request_list(requests);
		if(register_client_on_server(fd, server, request) < 0)
		{
			return -1;
		}
		gettimeofday(&start_request_time, NULL);
		
		requests->current_size = requests->current_size + 1;
		request->fd = fd;
		request->is_request_active = 1;
		request->request_start_time = start_request_time.tv_sec;
	}
	return 0;
}

int process_all_request(request_manager *manager, int timeout)
{
	if(process_events_on_server(manager->server, onread, onclose, onwrite, timeout) < 0)
	{
		return -1;
	}
	
	find_and_notify_all_request_timeout(manager);	
	return 0;
}

void reset_classifier_request_to_default_state(classifier_request *request)
{
	request->is_request_active = 0;
	request->current_state = HEADER_PARSING;
	request->current_packet_size = CLASSIFY_DOCUMENT_REQUEST_HEADER_SIZE;
	request->bytes_remaining_to_read = request->current_packet_size;
}

static classifier_request *get_request_not_active_from_request_list(classifier_request_list *requests)
{
	int i;
	for(i = 0;i < requests->capacity; i++)
	{
		if(requests->requests[i].is_request_active == 0)
		{
			printf("getting with index %d \n", i);
			return &requests->requests[i];
		}
	}	
	return NULL;
}
static void onwrite(void *data, file_server *server)
{
	classifier_request *request = (classifier_request *)data;
	request_manager *manager = (request_manager *)server->handler_data;
	REQUEST_WRITE_RESPONSE_CALLBACK onwrite_response = manager->onwrite_reponse_event;
	if(!IS_WRITE_STATE(request->current_state))
	{
		return;
	}
	onwrite_response(request, manager);
}

static void onread(void *data, file_server *server)
{
	classifier_request *request = (classifier_request *)data;
	request_manager *manager = (request_manager *)server->handler_data;
	REQUEST_ERROR_CALLBACK onrequest_error = manager->onrequest_error;
	REQUEST_CALLBACK onrequest_event = manager->onrequest_event;
	int bytes = 0;
	int keeping_reading = 0;
	if(!IS_READ_STATE(request->current_state))
	{
		return;		
	}
	bytes = read_request_data(request);
	keeping_reading =  bytes == request->bytes_remaining_to_read;
	if(bytes < 0)
	{
		onrequest_error(request, manager, SOCKET);
		return;
	}	
	request->bytes_remaining_to_read = request->bytes_remaining_to_read - bytes; 
	while(keeping_reading)
	{
	
		if(request->current_state == HEADER_PARSING)
		{	
			process_request_header(request, manager);
			if(!IS_READ_STATE(request->current_state))
			{
				return;
			}
		}
		else
		{
			process_request(request, manager);
		}
		bytes = read_request_data(request);
		keeping_reading =  bytes == request->bytes_remaining_to_read;
		if(bytes < 0)
		{
			onrequest_error(request, manager, SOCKET);
			return;
		}
		request->bytes_remaining_to_read = request->bytes_remaining_to_read - bytes; 
	}
}

static void process_request(classifier_request *request, request_manager *request_manager)
{
	REQUEST_CALLBACK onrequest_event = request_manager->onrequest_event;
	onrequest_event(request, request_manager, request->header.packet_type);
	request->current_packet_size = CLASSIFY_DOCUMENT_REQUEST_HEADER_SIZE;
	request->bytes_remaining_to_read = request->current_packet_size;
	request->current_state = HEADER_PARSING;
		
}



static void process_request_header(classifier_request *request, request_manager *request_manager)
{
	REQUEST_ERROR_CALLBACK onrequest_error = request_manager->onrequest_error;
	REQUEST_CALLBACK onrequest_event = request_manager->onrequest_event;
	if(parse_request_header(request->buffer->buffer, &request->header) < 0)
	{	
		onrequest_error(request, request_manager, PARSE);
		request->current_state = FINISH;
	}
	if(request->header.request_size > request->buffer->capacity)
	{		
		onrequest_error(request, request_manager, PACKET_SIZE);
		request->current_state = FINISH;
	}
	if(request->header.packet_type == DOCUMENT || request->header.packet_type ==  TITLE)
	{		
		request->current_state = BODY_PARSING;
		request->current_packet_size = request->header.request_size;
		request->bytes_remaining_to_read = request->header.request_size;
	}
	else
	{		
		onrequest_event(request, request_manager, END_REQUEST);
		request->current_state = FINISH;
	}	
}

static int read_request_data(classifier_request *request)
{
	int bytes_read = 0;
	int bytes_remaining_to_read = request->bytes_remaining_to_read;
	int offset = request->current_packet_size - bytes_remaining_to_read;
	char *char_buffer = request->buffer->buffer;
	char_buffer = char_buffer + offset;
	bytes_read = read_from_client(request->fd, char_buffer, bytes_remaining_to_read, REQUEST_TIMEOUT);
	return bytes_read;
}

static void onclose(void *data, file_server *server)
{
	classifier_request *request = (classifier_request *)data;
	request_manager *manager = (request_manager *)server->handler_data;
	REQUEST_ERROR_CALLBACK onrequest_error = manager->onrequest_error;
	onrequest_error(request, manager, PEER_CLOSE_CONNECTION);
}

static void find_and_notify_all_request_timeout(request_manager *request_manager)
{
	classifier_request_list *requests = request_manager->requests;
	int i;
	struct timeval current_time;
	time_t current_time_seconds;
	time_t time_diff;
	classifier_request *request = NULL;
	int timeout = request_manager->request_timeout;
	REQUEST_ERROR_CALLBACK onrequest_error = request_manager->onrequest_error;
	gettimeofday(&current_time, NULL);
	current_time_seconds = current_time.tv_sec;
	for(i = 0; i < requests->capacity; i++)
	{		
		request = &requests->requests[i];	
		if(request->is_request_active == 1)
		{
			
			time_diff = current_time_seconds - request->request_start_time;
			time_diff = time_diff * 1000;
			if(time_diff > request_manager->request_timeout)
			{
				onrequest_error(request, request_manager, REQUEST_TIMEOUT);
			}
			
		}
	}
}
