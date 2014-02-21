#ifndef CLASSIFIER_REQUEST_MANAGER_H
#define CLASSIFIER_REQUEST_MANAGER_H
#include "fileserver.h"
#include "memory_poll.h"
#include "protocol.h"
#include <sys/time.h>
#include "client_reader.h"
typedef enum{ HEADER_PARSING, BODY_PARSING, WRITING_RESPONSE,WRITING_END_RESPONSE ,FINISH } client_state;

typedef enum{ SOCKET, PEER_CLOSE_CONNECTION, REQUEST_TIMEOUT, PARSE, PACKET_SIZE} classifier_request_error;
#define CAN_HANDLE_MORE_REQUEST(requests)	((requests)->current_size < (requests)->capacity)
#define READ_TIME_OUT	500
#define IS_READ_STATE(state)	(state == HEADER_PARSING || state == BODY_PARSING)
#define IS_WRITE_STATE(state)	(state == WRITING_RESPONSE || state == WRITING_END_RESPONSE)
typedef struct
{
	int fd;
	int bytes_remaining_to_read;
	classify_document_request_header header;
	int current_packet_size;
	client_state current_state;	
	char_buffer *buffer;
	char is_request_active;
	void *request_data;
	time_t request_start_time;	
}classifier_request;

typedef struct
{
	classifier_request *requests;
	int capacity;
	int current_size;	
}classifier_request_list;

typedef struct classifier_request_manager
{
	void (* onrequest_event)(classifier_request *request, struct classifier_request_manager *request_manager, packet_type packet_type);

	void (* onwrite_reponse_event)(classifier_request *request, struct classifier_request_manager *request_manager);

	void (*onrequest_error)(classifier_request *request, struct classifier_request_manager *request_manager, classifier_request_error error);

	classifier_request_list *requests;
	int request_timeout;
	file_server *server;
}request_manager;

typedef void (*REQUEST_CALLBACK)(classifier_request *request, request_manager *manager, packet_type packet_type);

typedef void (*REQUEST_ERROR_CALLBACK)(classifier_request *request, request_manager *manager, classifier_request_error error);

typedef void (*REQUEST_WRITE_RESPONSE_CALLBACK)(classifier_request *request, request_manager *manager);


int start_server_and_register_event_handler(request_manager *request_manager, int max_request);

int add_news_request_to_request_manager(request_manager *request_manager);

int process_all_request(request_manager *request_manager, int timeout);

int close_request_on_request_manager(classifier_request *request, request_manager *request_manager);

void reset_classifier_request_to_default_state(classifier_request *request);

#endif
