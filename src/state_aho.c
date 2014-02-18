#include "state_aho.h"
state_aho_cache *load_state_aho(const char *_file_name,memory_page_buffer **_buffer)
{
	state_aho_cache *_state_aho_cache=NULL;
	GHashTable *_hcache=NULL;
	char *_base_address=NULL;	
	_state_aho_cache=(state_aho_cache *)malloc(sizeof(state_aho_cache));
	if(_state_aho_cache==NULL)
	{
		return NULL;
	}
	_hcache=g_hash_table_new(g_direct_hash,g_direct_equal);
	if(_hcache==NULL)
	{
		free(_state_aho_cache);
		return NULL;
	}
	_state_aho_cache->cache=_hcache;	
	_base_address=map_file(_file_name,_buffer,0);
	if(_base_address==NULL)
	{	
		delete_state_aho_cache(_state_aho_cache);
		return NULL;
	}
	fixed_pointers(_base_address,0);
	_state_aho_cache->state_aho=(state_aho*)_base_address;
	build_cache(_state_aho_cache);
	return _state_aho_cache;
}
void delete_state_aho_cache(state_aho_cache *_state_aho_cache)
{
	g_hash_table_destroy(_state_aho_cache->cache);
	free(_state_aho_cache);
}
void fixed_pointers(void *_base_pointer,uint64_t _offset)
{
	uint64_t _child_offset=0;
	uint64_t _parent_offset=0;
	uint64_t _fail_offset=0;
	uint32_t _size=0;
	uint32_t _i=0;
	char *_data=(char *)_base_pointer;
	state_aho *_state_aho=NULL;
	_state_aho=(state_aho *)(_data+_offset);
	_fail_offset=(uint64_t)(void *)_state_aho->fail;
	_parent_offset=(uint64_t)(void *)_state_aho->parent;
	_size=_state_aho->size;
	_state_aho->fail=(state_aho *)(_data+_fail_offset);
	_state_aho->parent=(state_aho *)(_data+_parent_offset);
	for(_i=0;_i<_size;_i++)
	{	
		_child_offset=(uint64_t)(void *)_state_aho->childs[_i];
/*		printf("child offset %" PRId64 "\n", _child_offset);*/
		_state_aho->childs[_i]=(state_aho *)(_data+_child_offset);
/*		if(_state_aho->parent->word_id==0  && _state_aho->word_id==1387 && _state_aho->childs[_i]->word_id==10473)
		{
			printf("********* size transitions %d *************\n",_state_aho->size);
		}*/
		fixed_pointers(_base_pointer,_child_offset);
	}
}
void build_cache(state_aho_cache *_state_aho_cache)
{
	state_aho *_state_aho=_state_aho_cache->state_aho;
	int _size=_state_aho->size;
	GHashTable *_hcache=_state_aho_cache->cache;
	state_aho *_state_aho_child=NULL;
	int _i=0;
	for(_i=0;_i<_size;_i++)
	{
		_state_aho_child=_state_aho->childs[_i];
/*		printf("*************** child word id %d ************\n",_state_aho_child->word_id);*/
		g_hash_table_insert(_hcache,GINT_TO_POINTER(_state_aho_child->word_id),_state_aho_child);
	}
}

void search_words(state_aho_cache *_state_aho_cache,word_poll *_word_poll)
{
	uint32_t _original_size=_word_poll->current_size;
	uint32_t _i=0;
	uint32_t _offset=_original_size;
	uint32_t _poll_size=_word_poll->poll_size;
	state_aho *_current_node=_state_aho_cache->state_aho;
	state_aho *_transition=NULL;
	while(_i<_original_size)
	{
		printf("************* search for word id %d **********\n",_word_poll->words[_i]);
		printf("********* current node %d ******\n",_current_node->word_id);
		_transition=get_transitions(_word_poll->words[_i],_current_node,_state_aho_cache);
		if(_transition==NULL)
		{
			printf("********* fail function for node %d ******\n",_current_node->word_id);
			_transition=find_node_transition_by_node_fail(_state_aho_cache,_current_node->fail,_word_poll->words[_i]);
		}
		if(_transition!=NULL)
		{
			if(_transition->has_result==1)
			{
				if(_offset>=_poll_size)
				{
					realloc_word_poll(_word_poll,_word_poll->page->memory_size+PAGE_SIZE);
					_poll_size=_word_poll->poll_size;
				}
				_word_poll->words[_offset]=_transition->value;
				printf("********** emit result %d ***********\n",_word_poll->words[_offset]);
				_offset=_offset+1;
				_word_poll->current_size=_word_poll->current_size+1;
			}
		}
		else
		{
			_transition=_state_aho_cache->state_aho;
		}
		_current_node=_transition;
		_i++;	
	}	
}


state_aho *find_node_transition_by_node_fail(state_aho_cache *_state_aho_cache,state_aho *_node_fail,uint32_t _word_id)
{
	state_aho *_node_transition=NULL;
	state_aho *_root_node=_state_aho_cache->state_aho;
	printf("********** search node transition by node fail for word id %d ******\n",_word_id);
	while(_root_node!=_node_fail)
	{
		printf("********** try with another node fail %d ******\n",_node_fail->word_id);
		
		_node_transition=get_transitions(_word_id,_node_fail,_state_aho_cache);
		if(_node_transition==NULL)
		{
			_node_fail=_node_fail->fail;
			continue;
		}
		printf("********** find node transition for word id %d ******\n",_word_id);
		return _node_transition;
	}
	return 	get_transitions(_word_id,_root_node,_state_aho_cache);
}
state_aho *get_transitions(uint32_t _word_id,state_aho *_current_node,state_aho_cache *_state_aho_cache)
{
	state_aho *_transition=NULL;
	GHashTable *_hcache=_state_aho_cache->cache;
	uint32_t _size=_current_node->size;
	uint32_t _i=0;
	printf("\n\n\n********** transitions for  word_id %d ********\n",_word_id);
	printf("**************** transitions size %d ****************\n",_size);
	if(_current_node==_state_aho_cache->state_aho)
	{
		if(g_hash_table_lookup_extended(_hcache,GINT_TO_POINTER(_word_id),NULL,(void *)&_transition)==TRUE)
		{		
			return _transition;	
		}
		return NULL;
	}
	for(_i=0;_i<_size;_i++)	
	{	
		printf("********** transitions word_id %d ********\n",_current_node->childs[_i]->word_id);
		if(_current_node->childs[_i]->word_id==_word_id)
		{				
			_transition=_current_node->childs[_i];
			break;
		}
	}
	return _transition;
}
