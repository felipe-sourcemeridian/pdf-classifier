#include "request_management.h"
request_classifier *create_request_classifier(memory_page_buffer **_page_buffer)
{
	request_classifier *_request_classifier=NULL;
	char_buffer *_page=NULL;
	word_poll *_word_poll=NULL;
	classifier_document *_classifier_document=create_classifier_document(_page_buffer);
	if(_classifier_document==NULL)
	{
		return NULL;
	}
	_page=create_char_buffer(_page_buffer);
	if(_page==NULL)
	{
		delete_classifier_document(_classifier_document);
		return NULL;
	}
	_word_poll=create_word_poll(_page_buffer);
	if(_word_poll==NULL)
	{
		delete_classifier_document(_classifier_document);
		delete_char_buffer(_page);
		return NULL;
	}
	_request_classifier=(request_classifier *)malloc(sizeof(request_classifier));
	if(_request_classifier==NULL)
	{
		delete_classifier_document(_classifier_document);
		delete_char_buffer(_page);
		delete_word_poll(_word_poll);
		return NULL;
	}
	memset(_request_classifier,0,sizeof(request_classifier));
	_request_classifier->word_poll=_word_poll;
	_request_classifier->classifier_document=_classifier_document;
	_request_classifier->page=_page;
	_request_classifier->fd=0;
	_request_classifier->enable=1;
	_request_classifier->state=HEADER;
	_request_classifier->current_size_packet=PACKET_HEADER_SIZE;
	_request_classifier->packet_type=CONVERT_TITLE;
	_request_classifier->bytes_remainder=PACKET_HEADER_SIZE;
	_request_classifier->offset=0;
	return _request_classifier;
}
void delete_request_classifier(request_classifier *_request_classifier)
{
	delete_classifier_document(_request_classifier->classifier_document);
	delete_char_buffer(_request_classifier->page);
	free(_request_classifier);
}
void clean_request_classifier(request_classifier *_request_classifier)
{
	_request_classifier->fd=0;
	_request_classifier->enable=1;
	_request_classifier->state=HEADER;
	_request_classifier->word_poll->current_size=0;
	_request_classifier->current_size_packet=PACKET_HEADER_SIZE;
	_request_classifier->packet_type=CONVERT_TITLE;
	_request_classifier->bytes_remainder=PACKET_HEADER_SIZE;
	_request_classifier->offset=0;
	clean_classifier_document(_request_classifier->classifier_document);
}
request_classifier_buffer *create_request_classifier_buffer(memory_page_buffer **_page_buffer,uint32_t _size)
{
	request_classifier_buffer *_buffer=(request_classifier_buffer *)malloc(sizeof(request_classifier_buffer));
	int _i=0;
	if(_buffer==NULL)
	{
		return NULL;
	}
	_buffer->buffer=(request_classifier **)malloc(sizeof(request_classifier *)*_size);
	if(_buffer->buffer==NULL)
	{
		free(_buffer);
		return NULL;
	}
	for(_i=0;_i<_size;_i++)
	{
		_buffer->buffer[_i]=create_request_classifier(_page_buffer);
		if(_buffer->buffer[_i]==NULL)
		{
			_buffer->capacity=0;	
			if(_i>0)
			{
				_buffer->capacity=_i+1;	
			}
			delete_request_classifier_buffer(_buffer);
			return NULL;
		}
	}
	_buffer->current_size=0;
	_buffer->capacity=_size;
	return _buffer;
}
void delete_request_classifier_buffer(request_classifier_buffer *_request_classifier_buffer)
{
	int _i=0;
	for(_i=0;_i<_request_classifier_buffer->capacity;_i++)
	{
		delete_request_classifier(_request_classifier_buffer->buffer[_i]);
	}
	free(_request_classifier_buffer->buffer);
	free(_request_classifier_buffer);
}
request_classifier_event *create_request_classifier_event(GKeyFile *_config_file,memory_page_buffer **_buffer,int *_request_event_classifier_status)
{
	int _timeout=0;
	int _max_events=0;
	int _epoll_fd=0;
	struct epoll_event *_epoll_events=NULL;
	request_classifier_buffer *_request_classifier_buffer=NULL;
	request_classifier_event *_request_classifier_event=NULL;
	_timeout=g_key_file_get_integer(_config_file,REQUEST_MANAGEMENT_GROUP_NAME,REQUEST_MANAGEMENT_WAIT_TIMEOUT,NULL);
	_max_events=g_key_file_get_integer(_config_file,REQUEST_MANAGEMENT_GROUP_NAME,REQUEST_MANAGEMENT_MAX_REQUEST_POOL,NULL);
	if(_timeout==0)
	{
		*_request_event_classifier_status=REQUEST_MANAGEMENT_TIMEOUT_IS_NOT_SET;
		return NULL;
	}
	if(_max_events==0)
	{
		*_request_event_classifier_status=REQUEST_MANAGEMENT_MAX_REQUEST_POOL_IS_NOT_SET;
		return NULL;
	}
	_epoll_events=(struct epoll_event *)calloc(_max_events,sizeof(struct epoll_event));
	if(_epoll_events == NULL)
	{
		*_request_event_classifier_status = REQUEST_MANAGEMENT_ALLOC;
		return NULL;
	}
	_epoll_fd = epoll_create(_max_events);
	if(_epoll_fd < 0)
	{
		*_request_event_classifier_status = REQUEST_MANAGEMENT_MAX_REQUEST_EPOLL_ERROR;
		return NULL;
	}
	_request_classifier_buffer=create_request_classifier_buffer(_buffer,_max_events);
	if(_request_classifier_buffer==NULL)
	{
		free(_epoll_events);
		*_request_event_classifier_status=REQUEST_MANAGEMENT_ALLOC;
		return NULL;
	}
	_request_classifier_event=(request_classifier_event *)malloc(sizeof(request_classifier_event));
	if(_request_classifier_event==NULL)
	{
		free(_epoll_events);
		delete_request_classifier_buffer(_request_classifier_buffer);
		*_request_event_classifier_status=REQUEST_MANAGEMENT_ALLOC;
		return NULL;	
	}	
	_request_classifier_event->epoll_events=_epoll_events;
	_request_classifier_event->epoll_fd=_epoll_fd;
	_request_classifier_event->buffer=_request_classifier_buffer;
	_request_classifier_event->timeout=_timeout;
	*_request_event_classifier_status=REQUEST_MANAGEMENT_OK;
	return _request_classifier_event;
}
void delete_request_classifier_event(request_classifier_event *_request_classifier_event)
{
	free(_request_classifier_event->epoll_events);
	delete_request_classifier_buffer(_request_classifier_event->buffer);
	close(_request_classifier_event->epoll_fd);
	free(_request_classifier_event);
}
void handle_request(request_classifier_event *_request_classifier_event)
{
	int _server_status=SERVER_OK;
	struct epoll_event _epoll_event;
	int _resp=0;
	int _event_size=0;
	int _i=0;
	struct epoll_event *_epoll_events=_request_classifier_event->epoll_events;
	int _epoll_fd=_request_classifier_event->epoll_fd;
	request_classifier_buffer *_buffer=_request_classifier_event->buffer;
	request_classifier *_request_classifier=NULL;
	int _max_request=_buffer->capacity;
	int _timeout=_request_classifier_event->timeout;
	server_info *_server_info=_request_classifier_event->server_info;
	syslog(LOG_INFO,"timeout  %f\n",_server_info->accept_timeout);
	while(_server_status==SERVER_OK)
	{
		if(_buffer->current_size<_buffer->capacity)
		{
			_resp=get_request(_server_info,_buffer->capacity-_buffer->current_size,&_server_status);
			if(_resp<0 && _server_status!=SERVER_OK)
			{
				syslog(LOG_ERR,"error to accept connection \n");
				break;
			}	
			for(_i=0;_i<_server_info->request_poll_current_size;_i++)
			{
				_epoll_event.data.fd=_server_info->request_poll[_i];
				_epoll_event.events=EPOLLIN  |EPOLLERR |EPOLLHUP | EPOLLET;
				_request_classifier=get_request_classifier_from_buffer(_buffer);
				_request_classifier->fd=_server_info->request_poll[_i];	
				_epoll_event.data.ptr=_request_classifier;
				_resp=epoll_ctl(_epoll_fd,EPOLL_CTL_ADD,_server_info->request_poll[_i],&_epoll_event);
				if(_resp==-1)
				{
					syslog(LOG_ERR,"error to add fd in epoll \n");
					break;
				}
				syslog(LOG_INFO,"request in queue %d new request for classifier fd %d\n",_server_info->request_poll_current_size,_request_classifier->fd);
			}	
		}
		_event_size=epoll_wait(_epoll_fd,_epoll_events,_max_request,_timeout);
		if(_event_size<0)
		{
			syslog(LOG_WARNING,"epoll error shutdown the server\n");
			break;
		}
		if(_event_size==0)
		{
			continue;
		}
		syslog(LOG_INFO,"connection active %d \n",_event_size);	
		handle_request_event(_epoll_events,_event_size,_request_classifier_event);
	}
}
void handle_request_event(struct epoll_event *_epoll_events,uint32_t _event_size,request_classifier_event *_request_classifier_event)
{
	uint32_t _i;
	request_classifier *_request_classifier=NULL;
	int _resp=0;
	convert_request *_convert_request=NULL;
	for(_i=0;_i<_event_size;_i++)
	{
		_request_classifier=(request_classifier *)_epoll_events[_i].data.ptr;
		if(((_epoll_events[_i].events & EPOLLERR) || (_epoll_events[_i].events & EPOLLHUP)) && !(_epoll_events[_i].events & EPOLLIN))
		{
			syslog(LOG_WARNING,"fd broken handle error fd--> %d\n",_request_classifier->fd);
			handle_request_error(_request_classifier_event,_request_classifier);
			continue;	
		}
		while((_resp=parse_request(_request_classifier))==REQUEST_MANAGEMENT_PARSE_OK)
		{
			if(_request_classifier->bytes_remainder==0 && _request_classifier->state==HEADER)
			{
				_convert_request=(convert_request *)_request_classifier->page->buffer;
				_request_classifier->bytes_remainder=_convert_request->size;
				_request_classifier->state=BODY;
				_request_classifier->packet_type=_convert_request->type;
				_request_classifier->offset=0;
				_request_classifier->page->size=_convert_request->size;
				if(_request_classifier->packet_type==CONVERT_END_DOCUMENT)
				{
					finish_request(_request_classifier_event,_request_classifier);
					break;
				}
				continue;
			}
			if(_request_classifier->bytes_remainder==0 && _request_classifier->state==BODY)
			{
				if(word_to_number(_request_classifier_event->file_transformation,_request_classifier->page,_request_classifier->word_poll)<0)
				{
					syslog(LOG_WARNING,"error transform word to number handle error for fd %d\n",_request_classifier->fd);
					handle_request_error(_request_classifier_event,_request_classifier);
					break;
				}
				if(_request_classifier->packet_type==CONVERT_TITLE)
				{
					_request_classifier->classifier_document->title_size=_request_classifier->word_poll->current_size;
				}
				_request_classifier->bytes_remainder=PACKET_HEADER_SIZE;
				_request_classifier->state=HEADER;
				_request_classifier->offset=0;
				_request_classifier->page->size=0;
			}
		}
		if(_resp==REQUEST_MANAGEMENT_PARSE_ERROR)
		{
			syslog(LOG_WARNING,"error reading fd %d handle error\n",_request_classifier->fd);
			handle_request_error(_request_classifier_event,_request_classifier);
			continue;		
		}
	}
}
void handle_request_error(request_classifier_event *_request_classifier_event,request_classifier *_request_classifier)
{
	struct epoll_event _epoll_event;
	int _fd=_request_classifier->fd;
	memset(&_epoll_event,0,sizeof(_epoll_event));
	_epoll_event.data.fd=_request_classifier->fd;
	_epoll_event.events=EPOLLHUP;
	epoll_ctl(_request_classifier_event->epoll_fd,EPOLL_CTL_DEL,_request_classifier->fd,&_epoll_event);
	close(_request_classifier->fd);	
	put_request_classifier_into_buffer(_request_classifier_event->buffer,_request_classifier);
	syslog(LOG_WARNING,"error closing %d connection \n",_fd);
}
void finish_request(request_classifier_event *_request_classifier_event,request_classifier *_request_classifier)
{
	classify(_request_classifier_event->classifier,_request_classifier->classifier_document,_request_classifier->word_poll);
	if(write_request_reponse(_request_classifier)<0)
	{
		syslog(LOG_ERR,"write response for fd call error handler %d\n",_request_classifier->fd);
		handle_request_error(_request_classifier_event,_request_classifier);
		return;
	}
	epoll_ctl(_request_classifier_event->epoll_fd,EPOLL_CTL_DEL,_request_classifier->fd,NULL);
	shutdown(_request_classifier->fd,2);
	close(_request_classifier->fd);
	put_request_classifier_into_buffer(_request_classifier_event->buffer,_request_classifier);	
}
int write_request_reponse(request_classifier *_request_classifier)
{
	node_score_buffer *_node_score_buffer=_request_classifier->classifier_document->node_score_buffer;
	node_score _node_score;
	int _fd=_request_classifier->fd;
	uint32_t _size=_node_score_buffer->current_size;
	uint32_t _i;
	char _data[400];
	uint32_t _write_bytes=0;
	_write_bytes=write(_fd,HEADER_RESPONSE,strlen(HEADER_RESPONSE));
	if(_write_bytes<=0)
	{	
		return -1;
	}
	syslog(LOG_INFO,"write response for fd %d\n",_request_classifier->fd);
	for(_i=0;_i<_size;_i++)
	{
		_node_score=_node_score_buffer->buffer[_i];	
		sprintf(_data,FORMAT_RESPONSE,_node_score.node_id,_node_score.frequency,_node_score.coverage,_node_score.frequency_top,_node_score.coverage_top,_node_score.fc,_node_score.fc15,_node_score.fc20,_node_score.flags,_node_score.roc);
		_write_bytes=write(_fd,_data,strlen(_data));
		if(_write_bytes<=0)
		{	
			return -1;
		}
	}
	return 0;
}
int parse_request(request_classifier *_request_classifier)
{
	int _bytes_read=0;
	int _size=_request_classifier->bytes_remainder;
	int _offset=_request_classifier->offset;
	_bytes_read=read(_request_classifier->fd,_request_classifier->page->buffer+_offset,_size);
	if(_bytes_read<0 && (errno!=EAGAIN && errno!=EWOULDBLOCK))
	{
		return REQUEST_MANAGEMENT_PARSE_ERROR;
	}
	if(_bytes_read<0)
	{
		return REQUEST_MANAGEMENT_PARSE_AGAIN;	
	}	
	_request_classifier->offset=_offset+_bytes_read;
	_request_classifier->bytes_remainder=_size-_bytes_read;
/*	syslog(LOG_INFO,"bytes read %d bytes remainder %d for fd %d\n",_bytes_read,_request_classifier->bytes_remainder,_request_classifier->fd);*/
	return REQUEST_MANAGEMENT_PARSE_OK;
}
request_classifier *get_request_classifier_from_buffer(request_classifier_buffer *_buffer)
{
	int _i=0;
	request_classifier *_request_classifier=NULL;
	for(_i=0;_i<_buffer->capacity;_i++)
	{
		_request_classifier=_buffer->buffer[_i];
		if(_request_classifier->enable==1)
		{
			_request_classifier->enable=0;
			_buffer->current_size=_buffer->current_size+1;
			return _request_classifier;
		}
	}
	return NULL;
}
void put_request_classifier_into_buffer(request_classifier_buffer *_buffer,request_classifier *_request_classifier)
{
	_buffer->current_size=_buffer->current_size-1;	
	clean_request_classifier(_request_classifier);
}
