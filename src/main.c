#include "signal.h"
#include "string.h"
#include "time.h"
#include "request_manager.h"
#include "request_manager_builder.h"
#include "errno.h"
#include "utility.h"
#include "daemon_util.h"
#include "config_util.h"
#include "file_transformation.h"
#include "classifier.h"
#include "load_files.h"
#include "unistd.h"

#define SERVER_CONFIG "server"
#define PORT	"port"
#define REQUEST_TIMEOUT_CONFIG	"request_timeout"
#define MAX_REQUEST	"max_request"
#define HEADER_RESPONSE "node id,document frequency,document coverage,summary frequency,summary coverage,fc,fc^15,fc^20,binary score,roc\n"
#define HEADER_RESPONSE_LENGTH strlen(HEADER_RESPONSE)
#define FORMAT_RESPONSE	"%zu,%f,%f,%f,%f,%f,%f,%f,%d,%d\n"
const char *REMOVE_ACCENTS = "NFD;[:Nonspacing Mark:] Remove;NFC;Lower";
const char *LOG_NAME="CLASSIFIER DAEMON";
int SERVER_FD=0;

typedef struct
{
	classifier_document *classifier_document;
	word_poll *word_poll;
	file_transformation *file_transformation;
	classifier *classifier;
}classifier_request_data;

void signal_callback(int _sig_num)
{
	syslog(LOG_INFO,"shutdown classifier server\n");
	close(SERVER_FD);	
}

static classifier_request_data *create_classifier_request_data(memory_page_buffer **buffer, file_transformation *file_transformation, classifier *classifier);

static int get_port_from_config_file(GKeyFile *config_file);

static int get_max_request_from_config_file(GKeyFile *config_file);

static int get_request_timeout_from_config_file(GKeyFile *config_file);

static void write_request_reponse(int fd, classifier_request_data *_request_classifier);
static  const char* get_error_description(classifier_request_error error);

classifier_request_list *create_requests(int max_request, memory_page_buffer **buffer, file_transformation *file_transformation, classifier *classifier);

static void close_and_clean_request(classifier_request *request, request_manager *manager);
void onrequest_event(classifier_request *request, request_manager *manager, packet_type type);

void onrequest_error(classifier_request *request, request_manager *manager,classifier_request_error error);


int main(int argc,char **argv)
{
	memory_page_buffer *_buffer=NULL;
	u16_char_buffer *_id=NULL;
	file_transformation *_file_transformation=NULL;
	classifier *_classifier=NULL;
	GHashTable *_hwords=NULL;
	GKeyFile *_config_file=NULL;
	char *_working_directory=NULL;
	char *_map_word_file=NULL;
	int _resp=0;
	int port;
	int max_request;
	int request_timeout;
	int event_timeout = 200;
	classifier_request_list *requests = NULL;
	request_manager *manager = NULL;
	if(argc==1)
	{
		printf("missing argument\n");
		exit(EXIT_FAILURE);
	}

	_config_file=load_config_file(argv[1]);
	if(_config_file==NULL)
	{
		printf("wrong config file please check\n");
		exit(EXIT_FAILURE);
	}
	_working_directory=g_key_file_get_string(_config_file,DAEMON_GROUP_NAME,DAEMON_WORKING_DIRECTORY, NULL);
	_map_word_file=g_key_file_get_string(_config_file,STATIC_DATA_GROUP_NAME,MAP_WORD_FILE,NULL);
	if(_working_directory == NULL)
	{
		printf("working directory dont set in config file\n");
		exit(EXIT_FAILURE);
	}
	if(_map_word_file==NULL)
	{
		printf("map word file is not set please check config file\n");
		exit(EXIT_FAILURE);
	}
	init_daemon(_working_directory);
	/* ignore signal broken pipen server shutdown when this things happen*/
	signal(SIGPIPE, SIG_IGN);
	if(signal(SIGINT,signal_callback)==SIG_ERR)
	{
		syslog(LOG_ERR,"signal handler error\n");
		exit(EXIT_FAILURE);
	}
	_id=create_id(REMOVE_ACCENTS);
	_classifier=build_classifier(_config_file,&_buffer);
	_hwords=load_words(_map_word_file,&_buffer);
	_file_transformation=create_file_transformation(&_buffer,_id,_hwords);
	if(_hwords==NULL)
	{
		syslog(LOG_ERR,"map words dont build please check the configuration file\n");
		exit(EXIT_FAILURE);
	}
	if(_file_transformation==NULL)
	{
		syslog(LOG_ERR,"build file transformation error please check the configuration\n");
		exit(EXIT_FAILURE);
	}
	if(_classifier==NULL)
	{
		syslog(LOG_ERR,"classifier build fail please check the configuration file\n");
		exit(EXIT_FAILURE);	
	}
	port = get_port_from_config_file(_config_file);
	max_request = get_max_request_from_config_file(_config_file);
	request_timeout = get_request_timeout_from_config_file(_config_file);
	requests = create_requests(max_request, &_buffer, _file_transformation, _classifier);
	syslog(LOG_ERR,"port: %d, max_request: %d, request_timeout: %d \n", port, max_request, request_timeout);
	manager = create_request_manager(port, request_timeout, max_request, requests, onrequest_event, onrequest_error);
	if(manager == NULL)
	{
		syslog(LOG_INFO,"error can not create request manager\n");
		exit(-1);
	}
	if(start_server_and_register_event_handler(manager, max_request) < 0)
	{			
		syslog(LOG_INFO, "error add new clients %s \n", strerror(errno));
		exit(-1);		
	}
	SERVER_FD = manager->server->fd;
	syslog(LOG_INFO,"server start\n");
	while(1)
	{
		if(add_news_request_to_request_manager(manager) < 0)
		{
			syslog(LOG_ERR, "error accept new connections %s \n", strerror(errno));
			exit(-1);
		}
		if(process_all_request(manager, event_timeout) < 0)
		{
			syslog(LOG_ERR,"error processing all request %s \n", strerror(errno));
			exit(-1);
		}		
	}
	syslog(LOG_INFO,"stop server\n");
	g_free(_map_word_file);
	g_free(_working_directory);
	delete_id(_id);
	delete_file_transformation(_file_transformation);
	delete_state_aho_cache(_classifier->state_aho_cache);
	g_hash_table_destroy(_hwords);
	g_hash_table_destroy(_classifier->map_term_node);
	g_hash_table_destroy(_classifier->map_term_weight_list);
	g_hash_table_destroy(_classifier->map_node_must_have);
	delete_unfiltered_node_list_cache(_classifier->unfiltered_node_list_cache);
	delete_classifier(_classifier);
	u_cleanup();
	free_memory_page_buffer(_buffer);
	closelog();
	return 0;
}
void onrequest_event(classifier_request *request, request_manager *manager, packet_type type)
{
	classifier_request_data *request_data = (classifier_request_data *)request->request_data;
	request->buffer->size = request->header.request_size;
	if(word_to_number(request_data->file_transformation, request->buffer, request_data->word_poll) < 0)
	{
		syslog(LOG_WARNING,"error transform word to number handle error for fd %d\n", request->fd);
		close_and_clean_request(request, manager);
	}

	if(request->header.packet_type == TITLE)
	{
		syslog(LOG_INFO,"request begin %d\n", request->fd);
		request_data->classifier_document->title_size = request_data->word_poll->current_size;
	}
	else if(request->header.packet_type == END_REQUEST)
	{
		classify(request_data->classifier, request_data->classifier_document, request_data->word_poll);
		syslog(LOG_INFO,"request end %d\n", request->fd);
		write_request_reponse(request->fd, request_data);
		sleep(10000);
		close_and_clean_request(request, manager);
		
	}
}

static void write_request_reponse(int fd, classifier_request_data *_request_classifier)
{
	
	node_score_buffer *_node_score_buffer = _request_classifier->classifier_document->node_score_buffer;
	node_score _node_score;
	uint32_t _size=_node_score_buffer->current_size;
	uint32_t _i;
	char _data[400];
	uint32_t _write_bytes=0;
	_write_bytes=write(fd, HEADER_RESPONSE, strlen(HEADER_RESPONSE));
	if(_write_bytes< 0)
	{	
		syslog(LOG_INFO,"error try to write response %s\n", strerror(errno));
		return;
	}

	for(_i=0;_i<_size;_i++)
	{
		_node_score=_node_score_buffer->buffer[_i];	
		sprintf(_data,FORMAT_RESPONSE,_node_score.node_id,_node_score.frequency,_node_score.coverage,_node_score.frequency_top,_node_score.coverage_top,_node_score.fc,_node_score.fc15,_node_score.fc20,_node_score.flags,_node_score.roc);
		_write_bytes=write(fd,_data,strlen(_data));
		syslog(LOG_INFO, "fd: %d, bytes :%d, writes bytes :%d, data: %s", fd, strlen(_data), _write_bytes, _data);
		if(_write_bytes <= 0)
		{	
			syslog(LOG_INFO,"error try to write response %s\n", strerror(errno));
			return ;
		}
	}
}

static void close_and_clean_request(classifier_request *request, request_manager *manager)
{
	classifier_request_data *request_data = (classifier_request_data *)request->request_data;
	clean_classifier_document(request_data->classifier_document);
	request_data->word_poll->current_size = 0;	
	close_request_on_request_manager(request, manager);
}

void onrequest_error(classifier_request *request, request_manager *manager, classifier_request_error error)
{
	syslog(LOG_ERR,"request begin %d %s\n", request->fd, get_error_description(error));
	if(error == SOCKET)
	{
		syslog(LOG_ERR,"request error  %d\ %s n", request->fd, strerror(errno));
	}
	close_request_on_request_manager(request, manager);
}


classifier_request_list *create_requests(int max_request, memory_page_buffer **page_buffer, file_transformation *file_transformation, classifier *classifier)
{
	char_buffer *buffer = NULL;
	int i;
	classifier_request *requests = (classifier_request *)calloc(max_request, sizeof(classifier_request));
	classifier_request_list *request_list = (classifier_request_list *) malloc(sizeof(classifier_request_list));
	if(request_list == NULL || requests == NULL)
	{
		syslog(LOG_ERR, "dont have memory viable build request list\n");
		exit(-1);
	}

	for(i = 0; i < max_request; i++)
	{
		buffer = create_char_buffer(page_buffer);		
		if(buffer == NULL)
		{
			syslog(LOG_ERR, "dont have memory viable build request list\n");		
			syslog(LOG_ERR,"error create buffer %s \n" ,strerror(errno));
			exit(-1);
		}
	
		requests[i].buffer= buffer;
		
		requests[i].request_data = create_classifier_request_data(page_buffer, file_transformation, classifier);		
		reset_classifier_request_to_default_state(&requests[i]);		
	}
	request_list->current_size = 0;
	request_list->capacity = max_request;
	request_list->requests = requests;
	return request_list;
		
}

static classifier_request_data *create_classifier_request_data(memory_page_buffer **buffer, file_transformation *file_transformation, classifier *classifier)
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

static int get_port_from_config_file(GKeyFile *config_file)
{
	int port = g_key_file_get_integer(config_file, SERVER_CONFIG, PORT, NULL);
	if(port == 0)
	{
		syslog(LOG_ERR,"the server port is not setup on configuration file, please setup the server port\n");
		exit(-1);		
	}
	return port;
}

static int get_max_request_from_config_file(GKeyFile *config_file)
{
	int max_request = g_key_file_get_integer(config_file,SERVER_CONFIG, MAX_REQUEST, NULL);
	if(max_request == 0)
	{
		syslog(LOG_ERR,"the max request is not setup on configuration file, please setup the server max request\n");
		exit(-1);			
	}
	return max_request;
}

static int get_request_timeout_from_config_file(GKeyFile *config_file)
{
	int timeout=g_key_file_get_integer(config_file, SERVER_CONFIG, REQUEST_TIMEOUT_CONFIG, NULL);
	if(timeout == 0)
	{
		syslog(LOG_ERR, "request timeout configurations is not setup on configuration file\n");
		exit(-1);
	}
	return timeout;
}
static const char* get_error_description(classifier_request_error error)
{
	switch(error)
	{
		case PARSE:
			return "Packet Type Is Wrong";
		case PACKET_SIZE:
			return "Packet Size is bigger than allowed";
		case PEER_CLOSE_CONNECTION:
			return "Peer Connection Closed";
		case REQUEST_TIMEOUT:
			return "Process Request is much that time allowd";
		default:
			return "System Error :";
	}
}
