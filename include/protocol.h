#ifndef PROTOCOL
#define PROTOCOL
#include <stdint.h>
#define CLASSIFY_DOCUMENT_REQUEST_HEADER_SIZE	8
typedef enum{TITLE = 1, DOCUMENT, END_REQUEST}packet_type;
typedef enum{DOCUMENT_RESPONSE = 126, END_RESPONSE = 127} packet_response_type;
typedef struct
{
	uint32_t request_size;
	unsigned char packet_type;
	char padding[3];
}classify_document_request_header;
#endif
