#ifndef CLASSIFY_DOCUMENT_REQUEST_PARSE
#define CLASSIFY_DOCUMENT_REQUEST_PARSE
#include "protocol.h"
#include "string.h"
int parse_request_header(void *data, classify_document_request_header *header);
#endif
