#include "memory_poll.h"
word_poll *create_word_poll(memory_page_buffer **_page_buffer)
{
	word_poll *_word_poll=(word_poll*)malloc(sizeof(word_poll));
	memory_page_list *_page=NULL;
	if(_word_poll==NULL)
	{
		return NULL;
	}
	_page=create_memory_page_list(WORD_POLL_MEMORY_SIZE,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,0);
	if(_page==NULL)
	{
		free(_word_poll);
		return NULL;
	}
	_word_poll->page=_page;
	_word_poll->current_size=0;
	*_page_buffer=add_page_to_buffer(*_page_buffer,_page);
	_word_poll->buffer=*_page_buffer;
	_word_poll->words=(uint32_t *)_page->addr;
	_word_poll->poll_size=WORD_POLL_MEMORY_SIZE/sizeof(uint32_t);
	return _word_poll;
}
void copy_word_poll(word_poll *_word_poll_source,word_poll *_word_poll_target,uint32_t _size,uint32_t _offset)
{
	memcpy(_word_poll_target->words,_word_poll_source->words+_offset,_size*sizeof(uint32_t));
	if(_size>_word_poll_source->current_size)
	{
		_word_poll_target->current_size=_word_poll_source->current_size;
	}
	else
	{
		_word_poll_target->current_size=_size;	
	}
}
word_poll *realloc_word_poll(word_poll *_word_poll,uint32_t _new_size)
{
	uint32_t _old_size=_word_poll->page->memory_size;
	_word_poll->page=realloc_memory_page(_word_poll->page,_new_size);
	if(_word_poll->page==NULL)
	{
		printf("********** realloc fail ************\n");
		return NULL;
	}
	_word_poll->words=(uint32_t *)_word_poll->page->addr;
	_word_poll->buffer->memory_size=_word_poll->buffer->memory_size-_old_size;
	_word_poll->poll_size=_word_poll->page->memory_size/sizeof(uint32_t);
	_word_poll->buffer->memory_size=_word_poll->buffer->memory_size+_new_size;
	return _word_poll;	
}
void delete_word_poll(word_poll *_word_poll)
{
	free(_word_poll);
}
char_buffer *create_char_buffer(memory_page_buffer **_page_buffer)
{
	memory_page_list *_page=NULL;
	char_buffer *_buffer=(char_buffer *)malloc(sizeof(char_buffer));
	if(_buffer==NULL)
	{
		return NULL;
	}
	_page=create_memory_page_list(CHAR_POLL_MEMORY_SIZE, PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,0);
	if(_page==NULL)
	{
		free(_buffer);
		return NULL;
	}
	*_page_buffer=add_page_to_buffer(*_page_buffer,_page);
	_buffer->buffer=(char *)_page->addr;
	_buffer->capacity=CHAR_POLL_MEMORY_SIZE;
	_buffer->size=0;
	_buffer->offset=0;
	return _buffer;
}
char_buffer *realloc_char_buffer(char_buffer *_buffer,uint32_t _new_size)
{
	uint32_t _old_size=_buffer->page->memory_size;
	_buffer->page=realloc_memory_page(_buffer->page,_new_size);
	if(_buffer->page==NULL)
	{
		return NULL;
	}
	_buffer->buffer=(char *)_buffer->page->addr;
	_buffer->page_buffer->memory_size=_buffer->page->memory_size-_old_size;
	_buffer->capacity=_buffer->page->memory_size/sizeof(uint32_t);
	_buffer->page_buffer->memory_size=_buffer->page->memory_size+_new_size;
	return _buffer;	
}
void delete_char_buffer(char_buffer *_buffer)
{
	free(_buffer);
}
u16_char_buffer *create_u16_char_buffer(memory_page_buffer **_page_buffer)
{
	memory_page_list *_page=NULL;
	u16_char_buffer *_buffer=(u16_char_buffer *)malloc(sizeof(u16_char_buffer));
	if(_buffer==NULL)
	{
		return NULL;
	}
	_page=create_memory_page_list(UCHAR_POLL_MEMORY_SIZE,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,0);
	if(_page==NULL)
	{
		free(_buffer);
		return NULL;
	}	
	*_page_buffer=add_page_to_buffer(*_page_buffer,_page);
	_buffer->data=(UChar *)_page->addr;
	_buffer->length=0;
	_buffer->capacity=(UCHAR_POLL_MEMORY_SIZE)/sizeof(UChar);
	_buffer->page=_page;
	return _buffer;
}
void delete_u16_char_buffer(u16_char_buffer* _buffer)
{
    free(_buffer);
}
node_score_buffer *create_node_score_buffer(memory_page_buffer **_buffer)
{
	node_score_buffer *_node_score_buffer=(node_score_buffer *)malloc(sizeof(node_score_buffer));
	memory_page_list *_page=NULL;
	if(_node_score_buffer==NULL)
	{
		return NULL;
	}	
	_page = create_memory_page_list(NODE_SCORE_POLL_MEMORY_SIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0);	
	if(_page == NULL)
	{
		free(_node_score_buffer);
		return NULL;
	}
	
	*_buffer=add_page_to_buffer(*_buffer,_page);
	_node_score_buffer->page=_page;
	_node_score_buffer->page_buffer=*_buffer;
	_node_score_buffer->buffer=(node_score *)_page->addr;
	_node_score_buffer->node_score_poll_size=NODE_SCORE_POLL_MEMORY_SIZE/sizeof(node_score);
	_node_score_buffer->current_size=0;
	return _node_score_buffer;
}
void delete_node_score_buffer(node_score_buffer *_node_score_buffer)
{
	free(_node_score_buffer);
}
node_score_buffer *realloc_node_score_buffer(node_score_buffer *_node_score_buffer,uint32_t _new_size)
{
	memory_page_list *_page=realloc_memory_page(_node_score_buffer->page,_new_size);
	uint32_t _old_size=_node_score_buffer->page_buffer->memory_size;
	if(_page==NULL)
	{
		delete_node_score_buffer(_node_score_buffer);
		return NULL;
	}
	_node_score_buffer->page=_page;	
	_node_score_buffer->buffer=(node_score *)_node_score_buffer->page->addr;
	_node_score_buffer->page_buffer->memory_size=_node_score_buffer->page->memory_size-_old_size;
	_node_score_buffer->node_score_poll_size=_node_score_buffer->page->memory_size/sizeof(node_score);
	_node_score_buffer->page_buffer->memory_size=_node_score_buffer->page->memory_size+_new_size;
	return _node_score_buffer;	
}
