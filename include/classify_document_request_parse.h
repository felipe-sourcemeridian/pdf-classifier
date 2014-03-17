#ifndef CLASSIFY_DOCUMENT_REQUEST_PARSE
#define CLASSIFY_DOCUMENT_REQUEST_PARSE
#include "protocol.h"
#include "string.h"
#ifdef __cplusplus
extern "C" {
#endif

int parse_request_header(void *data, classify_document_request_header *header);

#ifdef __cplusplus
}
#endif

#endif
