#include "load_files.h"
GHashTable *make_hash(const char *_file_name,memory_page_buffer **_buffer,GHashFunc _hashFunc,GEqualFunc _equalFunc,void **_data)
{
	GHashTable *_htable=NULL;
	_htable=g_hash_table_new(_hashFunc,_equalFunc);	
	
	if(_htable==NULL)
	{	
		return NULL;
	}
	*_data=map_file(_file_name,_buffer,1);
	if(*_data==NULL)
	{
		g_hash_table_destroy(_htable);
		return NULL;
	}
	return _htable;
}
GHashTable *load_words(const char *_file_name,memory_page_buffer **_buffer)
{
	uint32_t _word_size=0;	
	uint32_t _count=0;
	char *_data=NULL;
	GHashTable *_hwords=make_hash(_file_name,_buffer,g_str_hash,g_str_equal,((void **)&_data));
	word_struct* _word_struct=NULL;
	if(_hwords==NULL)
	{
		return NULL;
	}
	_word_size=*((uint32_t *)_data);
	_data=_data+4;	
	while(_count<_word_size)
	{	
		_count=_count+1;
		_word_struct=((word_struct*)(_data));	
		g_hash_table_insert(_hwords,_word_struct->word,GINT_TO_POINTER(_word_struct->word_id));
		_data=_data+(5+_word_struct->size);
	}	
	return _hwords;
}
GHashTable *load_node_sig_term(const char *_file_name,memory_page_buffer **_buffer)
{
	uint32_t _node_sig_term_size=0;
	char *_data=NULL;
	uint32_t _term_weight_size=sizeof(term_weight);
	uint32_t _term_weight_list_size=sizeof(term_weight_list);
	uint32_t _count=0;
	term_weight_list *_term_weight_list=NULL;	
	GHashTable *_hnode_sig_term=make_hash(_file_name,_buffer,g_direct_hash,g_direct_equal,((void **)&_data));
	if(_hnode_sig_term==NULL)
	{
		return NULL;
	}
	_node_sig_term_size=*((uint32_t *)_data);
	_data=_data+4;
	while(_count<_node_sig_term_size)
	{	
		_count=_count+1;
		_term_weight_list=((term_weight_list *)(_data));
		g_hash_table_insert(_hnode_sig_term,GINT_TO_POINTER(_term_weight_list->node_id),_term_weight_list);
		_data=_data+(_term_weight_list_size+(_term_weight_size*(_term_weight_list->size)));
	}
	return _hnode_sig_term;

}
GHashTable *load_list_node(const char *_file_name,memory_page_buffer **_buffer)
{
	uint32_t _list_node_size=0;
	char *_data=NULL;
	term_nodes *_term_nodes=NULL;
	uint32_t _code_list_node_size=sizeof(term_nodes);
	uint32_t _count=0;
	uint32_t _offset=0;
	GHashTable *_hlist_node=make_hash(_file_name,_buffer,g_direct_hash,g_direct_equal,((void **)&_data));
	if(_hlist_node==NULL)
	{
		return NULL;
	}
	_list_node_size=*((uint32_t *)_data);
	_data=_data+4;
	while(_count<_list_node_size)
	{
		_count=_count+1;
		_term_nodes=((term_nodes *)_data);
		_offset=4*(_term_nodes->size_code_list_node+_term_nodes->size_code_list_tax);
		_offset=_code_list_node_size+_offset;
		g_hash_table_insert(_hlist_node,GINT_TO_POINTER(_term_nodes->term_id),_term_nodes);
		_data=_data+_offset;
	}
	return _hlist_node;	
}
unfiltered_node_list *load_unfiltered_nodes(const char *_file_name,memory_page_buffer**_buffer)
{
	unfiltered_node_list *_unfiltered_node_list=NULL;
	char *_data=map_file(_file_name,_buffer,0);
	if(_data==NULL)
	{
		return NULL;
	}
	_unfiltered_node_list=(unfiltered_node_list *)_data;
	return _unfiltered_node_list;
}
GHashTable *load_node_musthave_term(const char *_file_name,memory_page_buffer **_buffer)
{
	uint32_t _node_musthave_term_size=0;
	char *_data=NULL;
	uint32_t _node_musthave_size_structure=sizeof(node_musthave_term);
	uint32_t _count=0;
	uint32_t _offset=0;
	node_musthave_term *_node_musthave_term=NULL;
	GHashTable *_hnode_musthave_term=make_hash(_file_name,_buffer,g_direct_hash,g_direct_equal,((void **)&_data));
	if(_hnode_musthave_term==NULL)
	{
		return NULL;
	}
	_node_musthave_term_size=*((uint32_t *)_data);
	_data=_data+4;
	while(_count<_node_musthave_term_size)
	{
		_count=_count+1;
		_node_musthave_term=(node_musthave_term *)_data;
		_offset=4*_node_musthave_term->size;
		_offset=_offset+_node_musthave_size_structure;
		g_hash_table_insert(_hnode_musthave_term,GINT_TO_POINTER(_node_musthave_term->node_id),_node_musthave_term);
		_data=_data+_offset;
	}
	return _hnode_musthave_term;
}
