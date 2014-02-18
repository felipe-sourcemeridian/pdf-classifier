#include "signal.h"
#include "string.h"
#include "time.h"
#include "request_management.h"
const char *REMOVE_ACCENTS = "NFD;[:Nonspacing Mark:] Remove;NFC;Lower";
const char *LOG_NAME="CLASSIFIER DAEMON";
int SERVER_FD=0;
void signal_callback(int _sig_num)
{
	syslog(LOG_INFO,"shutdown classifier server\n");
	close(SERVER_FD);	
}
void handler_server_error(int error)
{
	switch(error)
	{
		case SERVER_PORT_BUSY:
			syslog(LOG_ERR,"another application is running in the same port\n");
			break;
		case SERVER_ALLOC:
			syslog(LOG_ERR,"no memory viable\n");
			break;	
		case SERVER_FD_BAD:
			syslog(LOG_ERR,"bad file descriptor\n");
			break;
		case SERVER_PORT_IS_NOT_SET:
			syslog(LOG_ERR,"server port is not set in configuration file\n");
			break;	
		case SERVER_MAX_REQUEST_IS_NOT_SET:
			syslog(LOG_ERR,"server max request  is not set in configuration file\n");
			break;
		case SERVER_ACCEPT_TIMEOUT_IS_NOT_SET:
			syslog(LOG_ERR,"server accept timeout is not set in configuration file\n");
			break;
		default:
			syslog(LOG_ERR,"server unexpected error\n");
	}
	exit(EXIT_FAILURE);
}
void handler_request_management_error(int _error)
{
	switch(_error)
	{
		case REQUEST_MANAGEMENT_ALLOC:
			syslog(LOG_ERR,"request management memory not viable\n");
			break;	
		case REQUEST_MANAGEMENT_TIMEOUT_IS_NOT_SET:
			syslog(LOG_ERR,"request management timeout is not set in configuration file\n");
			break;	
		case REQUEST_MANAGEMENT_MAX_REQUEST_POOL_IS_NOT_SET:
			syslog(LOG_ERR,"request management timeout is not set in configuration file\n");
			break;
		case REQUEST_MANAGEMENT_MAX_REQUEST_EPOLL_ERROR:
			syslog(LOG_ERR,"request management epoll set error\n");
			break;
		default:
			syslog(LOG_ERR,"server unexpected error\n");
		exit(EXIT_FAILURE);
	}
}
int main(int argc,char **argv)
{
	memory_page_buffer *_buffer=NULL;
	u16_char_buffer *_id=NULL;
	file_transformation *_file_transformation=NULL;
	server_info *_server_info=NULL;
	classifier *_classifier=NULL;
	GHashTable *_hwords=NULL;
	GKeyFile *_config_file=NULL;
	char *_working_directory=NULL;
	char *_map_word_file=NULL;
	int _resp=0;
	request_classifier_event *_request_classifier_event=NULL;
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
	_working_directory=g_key_file_get_string(_config_file,DAEMON_GROUP_NAME,DAEMON_WORKING_DIRECTORY,NULL);
	_map_word_file=g_key_file_get_string(_config_file,STATIC_DATA_GROUP_NAME,MAP_WORD_FILE,NULL);
	if(_working_directory==NULL)
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
	/* ignore signal broken pipen server crass when this things happen*/
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
	_server_info=configure_server(_config_file,&_resp);
	if(_server_info==NULL)
	{
		handler_server_error(_resp);	
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
	_request_classifier_event=create_request_classifier_event(_config_file,&_buffer,&_resp);
	if(_request_classifier_event==NULL)
	{
		handler_request_management_error(_resp);
	}
	_request_classifier_event->server_info=_server_info;
	_request_classifier_event->file_transformation=_file_transformation;
	_request_classifier_event->classifier=_classifier;
	SERVER_FD=_server_info->fd_server;
	syslog(LOG_INFO,"server start\n");
	handle_request(_request_classifier_event);
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
	delete_server_info(_server_info);
	u_cleanup();
	free_memory_page_buffer(_buffer);
	closelog();
	return 0;
}
