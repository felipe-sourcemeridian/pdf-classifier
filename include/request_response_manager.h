#ifndef REQUEST_RESPONSE_MANAGER
#define REQUEST_RESPONSE_MANAGER
#include "protocol.h"
#include "request_manager.h"
#include "client_writer.h"
#include "classifier_client.h"
#include <syslog.h>
#define HEADER_RESPONSE "node id,document frequency,document coverage,summary frequency,summary coverage,fc,fc^15,fc^20,binary score,roc\n"
#define HEADER_RESPONSE_LENGTH strlen(HEADER_RESPONSE)
#define FORMAT_RESPONSE	"%zu,%f,%f,%f,%f,%f,%f,%f,%d,%d\n"
void process_request_response(classifier_request *request, request_manager *manager);

void set_request_response_state_default(classifier_request *request);

int process_write_request_data(classifier_request *request, request_manager *manager);
#endif
