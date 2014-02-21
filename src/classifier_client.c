#include "classifier_client.h"
classifier_request_data *create_classifier_request_data(memory_page_buffer **buffer, file_transformation *file_transformation, classifier *classifier)
{
	classifier_document * document = create_classifier_document(buffer);
	word_poll *word_poll= NULL;
	classifier_request_data *request_data = (classifier_request_data *)malloc(sizeof(classifier_request_data));
	word_poll = create_word_poll(buffer);
	
	if(document == NULL || word_poll == NULL || request_data == NULL)
	{
		syslog(LOG_ERR, "dont have memory viable build request list\n");
		exit(-1);
	}
	
	request_data->word_poll = word_poll;
	request_data->classifier_document = document;
	request_data->classifier = classifier;
	request_data->file_transformation = file_transformation;
	return request_data;
}

void close_and_clean_request(classifier_request *request, request_manager *manager)
{
	classifier_request_data *request_data = (classifier_request_data *)request->request_data;
	clean_classifier_document(request_data->classifier_document);
	request_data->word_poll->current_size = 0;	
	close_request_on_request_manager(request, manager);
}

