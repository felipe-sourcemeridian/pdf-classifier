#include "classify_document_request_parse.h"
int parse_request_header(void *data, classify_document_request_header *header)
{
	memcpy(header, data, sizeof(classify_document_request_header));
	if(header->packet_type > 3)
	{
		return -1;
	}
	return 0;
}
