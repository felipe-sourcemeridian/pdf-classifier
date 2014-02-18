#ifndef REQUEST_MANAGEMENT_H
#define REQUEST_MANAGEMENT_H
#define REQUEST_MANAGEMENT_ALLOC	0
#define REQUEST_MANAGEMENT_OK	1
#define REQUEST_MANAGEMENT_TIMEOUT_IS_NOT_SET	2
#define REQUEST_MANAGEMENT_MAX_REQUEST_POOL_IS_NOT_SET	4
#define REQUEST_MANAGEMENT_MAX_REQUEST_EPOLL_ERROR	8
#define REQUEST_MANAGEMENT_PARSE_AGAIN	16
#define REQUEST_MANAGEMENT_PARSE_OK	32
#define REQUEST_MANAGEMENT_PARSE_ERROR	64
#define HEADER_RESPONSE "node id,document frequency,document coverage,summary frequency,summary coverage,fc,fc^15,fc^20,binary score,roc\n"
#define HEADER_RESPONSE_LENGTH strlen(HEADER_RESPONSE)
#define FORMAT_RESPONSE	"%zu,%f,%f,%f,%f,%f,%f,%f,%d,%d\n"
#include "file_server.h"
#include "sys/epoll.h"
#include "string.h"
#include "time.h"
#include "utility.h"
#include "daemon_util.h"
#include "config_util.h"
#include "file_transformation.h"
#include "classifier.h"
#include "load_files.h"
typedef enum{HEADER=0,BODY=1} state_parse;
typedef struct
{	
	uint32_t current_size_packet;	
	uint32_t bytes_remainder;
	uint32_t offset;
	uint32_t enable;
	int fd;
	state_parse state;
	unsigned char packet_type;
	char_buffer *page;
	classifier_document *classifier_document;
	word_poll *word_poll;
}request_classifier;

typedef struct
{
	uint32_t current_size;
	uint32_t capacity;
	request_classifier **buffer;
}request_classifier_buffer;

typedef struct
{
	struct epoll_event *epoll_events;
	request_classifier_buffer *buffer;
	server_info *server_info;
	classifier *classifier;
	file_transformation *file_transformation;
	int epoll_fd;
	int max_events;
	int timeout;
}request_classifier_event;

request_classifier *create_request_classifier(memory_page_buffer **_page_buffer);

void delete_request_classifier(request_classifier *_request_classifier);
request_classifier_buffer *create_request_classifier_buffer(memory_page_buffer **_page_buffer,uint32_t _size);

void delete_request_classifier_buffer(request_classifier_buffer *_request_classifier_buffer);

request_classifier_event *create_request_classifier_event(GKeyFile *_config_file,memory_page_buffer **_buffer,int *_request_event_classifier_status);

void delete_request_classifier_event(request_classifier_event *_request_classifier_event);

void handle_request(request_classifier_event *_request_classifier_event);

void clean_request_classifier(request_classifier *_request_classifier);

int parse_request(request_classifier *_request_classifier);

request_classifier *get_request_classifier_from_buffer(request_classifier_buffer *_buffer);

void finish_request(request_classifier_event *_request_classifier_event,request_classifier *_request_classifier);

void put_request_classifier_into_buffer(request_classifier_buffer *_buffer,request_classifier *_request_classifier);

void handle_request_event(struct epoll_event *_epoll_events,uint32_t _event_size,request_classifier_event *_request_event_classifier);

void handle_request_error(request_classifier_event *_request_classifier_event,request_classifier *_request_classifier);

int write_request_reponse(request_classifier *_request_classifier);
#endif
