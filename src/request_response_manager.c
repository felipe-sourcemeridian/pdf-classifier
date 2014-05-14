#include "request_response_manager.h"


static void write_response_to_request_buffer(classifier_request *request, const char *response, int length);

static void write_header_response_to_request_buffer(classifier_request *request, unsigned char packet_type, int length);


static int write_request_data(classifier_request *request);

void process_request_response(classifier_request *request, request_manager *manager) {
    classifier_request_data *request_data = (classifier_request_data *) request->request_data;
    node_score_buffer *node_score_buffer = request_data->classifier_document->node_score_buffer;
    node_score node_score_element;
    uint32_t size = node_score_buffer->current_size;
    uint32_t i = request_data->current_node_core;
    char temporal_buffer[400];
    int keep_writing = process_write_request_data(request, manager);
    if (keep_writing < 0) {
        return;
    }
    while ((i < size) && (keep_writing == 1)) {
        node_score_element = node_score_buffer->buffer[i];
        sprintf(temporal_buffer, FORMAT_RESPONSE, node_score_element.node_id, node_score_element.tax_id, node_score_element.frequency, node_score_element.coverage, node_score_element.frequency_top, node_score_element.coverage_top, node_score_element.fc, node_score_element.fc15, node_score_element.fc20, node_score_element.flags, node_score_element.roc);
        write_response_to_request_buffer(request, temporal_buffer, strlen(temporal_buffer));
        i++;
        request_data->current_node_core = i;
        request_data->bytes_remaining_to_response = strlen(temporal_buffer) + CLASSIFY_DOCUMENT_REQUEST_HEADER_SIZE;
        request_data->current_response_size = request_data->bytes_remaining_to_response;
        keep_writing = process_write_request_data(request, manager);

        if (keep_writing < 0) {
            return;
        }

    }
    syslog(LOG_ERR, "current nodes writes %d nodes remain %d, keep writing %d\n", request_data->current_node_core, size, keep_writing);
    if (request_data->current_node_core >= size && keep_writing == 1) {
        write_header_response_to_request_buffer(request, END_RESPONSE, 0);
        request_data->bytes_remaining_to_response = CLASSIFY_DOCUMENT_REQUEST_HEADER_SIZE;
        request_data->current_response_size = request_data->bytes_remaining_to_response;
        syslog(LOG_ERR, "fd :%d write end response packet \n", request->fd);
        request->current_state = WRITING_END_RESPONSE;
        process_write_request_data(request, manager);
    }

}

int process_write_request_data(classifier_request *request, request_manager *manager) {
    classifier_request_data *request_data = (classifier_request_data *) request->request_data;
    int bytes_written = write_request_data(request);
    int keep_writing = bytes_written == request_data->bytes_remaining_to_response;
    if (bytes_written < 0) {
        syslog(LOG_INFO, "error try to write response close connection %s\n", strerror(errno));
        close_and_clean_request(request, manager);
        return -1;
    }
    request_data->bytes_remaining_to_response -= bytes_written;
    return keep_writing;

}

static int write_request_data(classifier_request *request) {
    classifier_request_data *request_data = (classifier_request_data *) request->request_data;
    int bytes_written = 0;
    int bytes_remaining_to_response = request_data->bytes_remaining_to_response;
    int offset = request_data->current_response_size - bytes_remaining_to_response;
    char *buffer_char = request->buffer->buffer;
    buffer_char = buffer_char + offset;
    if (offset > 0) {
        syslog(LOG_ERR, "buffer offet %d %s\n", offset, buffer_char);
        syslog(LOG_ERR, "data: ->  %s\n", buffer_char);
    }

    bytes_written = write_to_client(request->fd, buffer_char, bytes_remaining_to_response);
    return bytes_written;
}

void set_request_response_state_default(classifier_request *request) {

    classifier_request_data *request_data = (classifier_request_data *) request->request_data;
    request_data->current_response_size = HEADER_RESPONSE_LENGTH + CLASSIFY_DOCUMENT_REQUEST_HEADER_SIZE;
    request_data->current_node_core = 0;
    request_data->bytes_remaining_to_response = request_data->current_response_size;
    write_response_to_request_buffer(request, HEADER_RESPONSE, HEADER_RESPONSE_LENGTH);
}

static void write_response_to_request_buffer(classifier_request *request, const char *response, int length) {
    char_buffer *buffer = request->buffer;
    write_header_response_to_request_buffer(request, DOCUMENT_RESPONSE, length);
    memcpy((buffer->buffer + CLASSIFY_DOCUMENT_REQUEST_HEADER_SIZE), response, length);

}

static void write_header_response_to_request_buffer(classifier_request *request, unsigned char type, int length) {
    char_buffer *buffer = request->buffer;
    classify_document_request_header header;
    header.request_size = length;
    header.packet_type = type;
    header.padding[0] = 10;
    header.padding[1] = 11;
    header.padding[2] = 8;
    memcpy(buffer->buffer, &header, CLASSIFY_DOCUMENT_REQUEST_HEADER_SIZE);
}
