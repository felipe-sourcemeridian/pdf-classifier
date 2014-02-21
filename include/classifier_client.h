#ifndef CLASSIFIER_CLIENT
#define CLASSIFIER_CLIENT
#include "memory_poll.h"
#include "request_manager.h"
#include "file_transformation.h"
#include "classifier.h"
#include "syslog.h"

typedef struct
{
	classifier_document *classifier_document;
	word_poll *word_poll;
	file_transformation *file_transformation;
	classifier *classifier;
	int bytes_remaining_to_response;
	int current_response_size;	
	int current_node_core;
}classifier_request_data;

classifier_request_data *create_classifier_request_data(memory_page_buffer **buffer, file_transformation *file_transformation, classifier *classifier);

void close_and_clean_request(classifier_request *request, request_manager *manager);
#endif
