#ifndef CLASSIFIER_REQUEST_MANAGER_BUILDER
#define CLASSIFIER_REQUEST_MANAGER_BUILDER
#include "request_manager.h"
request_manager *create_request_manager(int port, int timeout, int max_request, classifier_request_list *requests, REQUEST_CALLBACK onrequest_event, REQUEST_ERROR_CALLBACK onrequest_error, REQUEST_WRITE_RESPONSE_CALLBACK onwrite_response);
#endif
