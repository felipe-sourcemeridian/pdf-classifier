#include "stdlib.h"
#include <stdio.h>
#include "errno.h"
#include "request_manager.h"
#include "request_manager_builder.h"
#include "memory_poll.h"
#define MESSAGE_RESPONSE "request finish sucess"

void print_header_request(classify_document_request_header header);


char *get_packet_description_from_header(classify_document_request_header header);

classifier_request_list *create_requests(int max_request, memory_page_buffer **buffer);

void onrequest_event(classifier_request *request, request_manager *manager, packet_type type);

void onrequest_error(classifier_request *request, request_manager *manager,classifier_request_error error);

void onrequest_write_response_event(classifier_request *request, request_manager *manager);

void print_error_description(classifier_request_error error);

int main(int argc, char **argv)
{	
	int max_request = 2;
	int port = 5000;
	int request_timeout = 10000;
	int event_timeout = 100;
	memory_page_buffer *buffer = NULL;
	request_manager *manager = NULL;
	classifier_request_list *request_list = create_requests(max_request, &buffer);
	if(request_list == NULL)
	{
		printf("error building request list \n");
		exit(-1);		
	}
	manager = create_request_manager(port, request_timeout, max_request, request_list, onrequest_event, onrequest_error, onrequest_write_response_event);
	if(manager == NULL)
	{
		printf("error building request manager \n");
		exit(-1);
	}	
	if(start_server_and_register_event_handler(manager, max_request) < 0)
	{
		printf("error starting server %s \n",strerror(errno));
		exit(-1);	
	}
	while(1)
	{

		if(add_news_request_to_request_manager(manager) < 0)
		{
			printf("error accept new connections %s \n", strerror(errno));
			exit(-1);
		}
		if(process_all_request(manager, event_timeout) < 0)
		{
			printf("error processing all request %s \n", strerror(errno));
			exit(-1);
		}		
		
	}
		
}

void onrequest_event(classifier_request *request, request_manager *manager, packet_type type)
{
	char *buffer_char = request->buffer->buffer;
	buffer_char[request->header.request_size] = 0;
	print_header_request(request->header);
	printf("receiving request type :%s with content :%s fd: %d\n", get_packet_description_from_header(request->header), buffer_char, request->fd);
	if(request->header.packet_type == END_REQUEST)
	{
		close_request_on_request_manager(request, manager);
	}
}

void onrequest_error(classifier_request *request, request_manager *manager, classifier_request_error error)
{
	printf("error on fd: %d\n",request->fd);
	print_error_description(error);
	close_request_on_request_manager(request, manager);
}

classifier_request_list *create_requests(int max_request, memory_page_buffer **page_buffer)
{
	char_buffer *buffer = NULL;
	int i;
	classifier_request *requests = (classifier_request *)calloc(max_request, sizeof(classifier_request));
	classifier_request_list *request_list = (classifier_request_list *) malloc(sizeof(classifier_request_list));
	if(request_list == NULL)
	{
		return NULL;
	}

	if(requests == NULL)
	{
		return NULL;
	}

	for(i = 0; i < max_request; i++)
	{
		buffer = create_char_buffer(page_buffer);
		if(buffer == NULL)
		{
			return NULL;
		}
		requests[i].buffer = buffer;	
		reset_classifier_request_to_default_state(&requests[i]);
	}
	request_list->current_size = 0;
	request_list->capacity = max_request;
	request_list->requests = requests;
	return request_list;
		
}

void print_header_request(classify_document_request_header header)
{
	printf("packet size :%d \n",header.request_size);
	printf("packet type :%d\n", header.packet_type);
}

void onrequest_write_response_event(classifier_request *request, request_manager *manager)
{
	printf("buffer viable for writing \n");
}

void print_error_description(classifier_request_error error)
{
	char *error_name = "";
	char *desc = "";
	switch(error)
	{
		case PARSE:
			error_name = "Packet Type Is Wrong";
			break;
		case PACKET_SIZE:
			error_name = "Packet Size is bigger than allowed";
			break;
		case PEER_CLOSE_CONNECTION:
			error_name = "Peer Connection Closed";
			break;
		case REQUEST_TIMEOUT:
			error_name = "Process Request is much that time allowd";
			break;
		default:
			error_name = "System Error :";
			desc = strerror(errno);
	}
	printf("%s %s \n",error_name, desc);
}

char *get_packet_description_from_header(classify_document_request_header header)
{
	if(header.packet_type == TITLE)
	{
		return "Titlte Packet";
	}else if(header.packet_type == DOCUMENT)
	{
		return "Document Content Packet";
	}
	return "End Request Packet";
}
