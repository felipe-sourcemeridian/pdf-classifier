#include "score_document.h"
void map_node(GHashTable *_hnode,uint32_t *_words,uint32_t _size)
{
	uint32_t _i;
	for(_i=0;_i<_size;_i++)
	{
		g_hash_table_insert(_hnode,GINT_TO_POINTER(_words[_i]),GINT_TO_POINTER(_words[_i]));
	}
}

int search_node_by_terms(classifier *_classifier,classifier_document *_classifier_document)
{
	term_nodes *_term_nodes=NULL;
	gpointer _hash_value=NULL;
	gpointer _original_value=NULL;
	GHashTable *_map_term_node=_classifier->map_term_node;
	GHashTable *_map_node=_classifier_document->map_node;
	GHashTable *_map_tax_node=_classifier_document->map_tax_node;
	GList *_list_word_document=g_hash_table_get_keys(_classifier_document->map_document);
	GList *_list_word_document_original=_list_word_document;
	for(;_list_word_document!=NULL;_list_word_document=_list_word_document->next)
	{
		if(g_hash_table_lookup_extended(_map_term_node,_list_word_document->data,&_hash_value,&_original_value)==FALSE)
		{
			continue;
		}	
		_term_nodes=(term_nodes *)_original_value;
		map_node(_map_node,_term_nodes->code_list_buffer,_term_nodes->size_code_list_node);
		map_node(_map_tax_node,_term_nodes->code_list_buffer+_term_nodes->size_code_list_node,_term_nodes->size_code_list_tax);
	}
	g_list_free(_list_word_document_original);
	return 0;
}

int intersection_nodes(classifier *_classifier,classifier_document *_classifier_document)
{
	GList *_list_node_id=g_hash_table_get_keys(_classifier_document->map_node);
	GList *_list_node_original=_list_node_id;
	GHashTable *_map_tax_node=_classifier_document->map_tax_node;
	GHashTable *_map_unfiltered_node_list=_classifier->unfiltered_node_list_cache->map_unfiltered_node_list;
	gpointer _hash_value=NULL;
	gpointer _original_value=NULL;
	uint32_t _index=0;
	node_score_buffer *_node_score_buffer=_classifier_document->node_score_buffer;
	for(;_list_node_id!=NULL;_list_node_id=_list_node_id->next)
	{
		if(g_hash_table_lookup_extended(_map_tax_node,_list_node_id->data,&_hash_value,&_original_value)==FALSE && g_hash_table_lookup_extended(_map_unfiltered_node_list,_list_node_id->data,&_hash_value,&_original_value)==FALSE)
		{
			continue;
		}
		if(_node_score_buffer->current_size==_node_score_buffer->node_score_poll_size)
		{
			_node_score_buffer=realloc_node_score_buffer(_node_score_buffer,_node_score_buffer->page->memory_size+PAGE_SIZE);
			if(_node_score_buffer==NULL)
			{
				g_list_free(_list_node_original);
				return -1;
			}
		}
		memset(&_node_score_buffer->buffer[_index],0,sizeof(node_score));
		_node_score_buffer->buffer[_index].node_id=GPOINTER_TO_INT(_list_node_id->data);
		_index=_index+1;
		_node_score_buffer->current_size=_index;
	}
	g_list_free(_list_node_original);
	return 0;
}

int score_classifier_document(classifier *_classifier,classifier_document *_classifier_document)
{
	term_weight_list *_term_weight_list = NULL;	
	gpointer _hash_value = NULL;
	gpointer _original_value = NULL;
	node_score_buffer *_node_score_buffer = _classifier_document->node_score_buffer;
	GHashTable *_map_term_weight_list = _classifier->map_term_weight_list;
	uint32_t _size = _classifier_document->node_score_buffer->current_size;
	uint32_t _i;
	uint32_t _node_id = 0;
	for(_i=0;_i<_size;_i++)
	{
		_node_id=_node_score_buffer->buffer[_i].node_id;
		if(g_hash_table_lookup_extended(_map_term_weight_list,GINT_TO_POINTER(_node_id),&_hash_value,&_original_value)==FALSE)
		{
			continue;
		}
		_term_weight_list=(term_weight_list *)_original_value;
		_node_score_buffer->buffer[_i].type=_term_weight_list->type;
		do_score(_term_weight_list,_classifier_document->word_poll_document,_classifier_document->map_document,&_node_score_buffer->buffer[_i].frequency,&_node_score_buffer->buffer[_i].coverage, _classifier_document->document_size);
		do_score(_term_weight_list,_classifier_document->word_poll_summary,_classifier_document->map_summary,&_node_score_buffer->buffer[_i].frequency_top,&_node_score_buffer->buffer[_i].coverage_top, SUMMARY_SIZE);
	}
	return 0;
}

void do_score(term_weight_list *_term_weight_list,word_poll *_word_poll,GHashTable *_map,float *_frequency,float *_coverage, uint32_t document_size)
{
	uint32_t _term_weight_size=_term_weight_list->size;
	uint32_t _word_poll_size=_word_poll->current_size;
	uint32_t _index_term_weight_buffer=0;
	uint32_t _index_word_poll=0;
	uint32_t _word_count = 0;
	float _frequency_sum = 0.0f;
    	float _frequency_aux = 0.0f;
    	float _coverage_aux = 0.0f;
	term_weight *_term_weight_buffer=_term_weight_list->term_weight_buffer;
	while(_index_term_weight_buffer<_term_weight_size && _index_word_poll<_word_poll_size)
	{
		if(_word_poll->words[_index_word_poll]==_term_weight_buffer[_index_term_weight_buffer].term_id)
		{
			_word_count=GPOINTER_TO_INT(g_hash_table_lookup(_map,GINT_TO_POINTER(_word_poll->words[_index_word_poll])));
			_frequency_aux=(float)_word_count/document_size;
			_frequency_aux=_frequency_aux*_term_weight_buffer[_index_term_weight_buffer].weight;
			_frequency_sum=_frequency_sum+_frequency_aux;
			_coverage_aux=_coverage_aux+1.0f;
		}
		if(_word_poll->words[_index_word_poll]<_term_weight_buffer[_index_term_weight_buffer].term_id)
		{
			_index_word_poll=_index_word_poll + 1;
		}
		else
		{
			_index_term_weight_buffer = _index_term_weight_buffer + 1;
		}
	}
	*_frequency = _frequency_sum * 1000.0f;
	*_coverage = (float)_coverage_aux / _term_weight_list->size;
}

void do_score_binary(word_poll *_word_poll,classifier *_classifier,classifier_document *_classifier_document,NODE_SCORE_FLAG _flags)
{
	uint32_t _size = _classifier_document->node_score_buffer->current_size;
	node_score_buffer *_node_score_buffer=_classifier_document->node_score_buffer;
	GHashTable *_map_node_must_have=_classifier->map_node_must_have;
	uint32_t _node_id=0;
	uint32_t _i;
	uint32_t _index_node_term=0;
	uint32_t _index_word_poll=0;
	uint32_t _word_poll_size=_word_poll->current_size;
	gpointer _hash_value=NULL;
	gpointer _original_value=NULL;
	node_musthave_term *_node_must_have_term=NULL;
	for(_i=0;_i<_size;_i++)
	{
		_node_id=_node_score_buffer->buffer[_i].node_id;
		if(g_hash_table_lookup_extended(_map_node_must_have,GINT_TO_POINTER(_node_id),&_hash_value,&_original_value)==FALSE)
		{
			continue;
		}
		_node_must_have_term=(node_musthave_term *)_original_value;
		_index_node_term = 0;
		_index_word_poll = 0;
		while(_index_word_poll<_word_poll_size && _index_node_term<_node_must_have_term->size)
		{
			if(_word_poll->words[_index_word_poll]==_node_must_have_term->words[_index_node_term])
			{
				_node_score_buffer->buffer[_i].flags=_node_score_buffer->buffer[_i].flags|_flags;
				break;
			}
			if(_word_poll->words[_index_word_poll]<_node_must_have_term->words[_index_node_term])
			{
				_index_word_poll=_index_word_poll+1;
			}
			else
			{
				_index_node_term=_index_node_term+1;
			}
		}
	}
}

void do_combine_score(classifier_document *_classifier_document)
{
	node_score_buffer *_node_score_buffer=_classifier_document->node_score_buffer;
	uint32_t _size=_node_score_buffer->current_size;
	uint32_t _i;
	float _frequency;
	float _coverage;
	float _ftimesc;
	for(_i=0;_i<_size;_i++)
	{
		_frequency=_node_score_buffer->buffer[_i].frequency;
		_coverage=_node_score_buffer->buffer[_i].coverage;
		_ftimesc=_frequency*_coverage;
		_node_score_buffer->buffer[_i].fc=_ftimesc;
		_node_score_buffer->buffer[_i].fc15=0.0;
		_node_score_buffer->buffer[_i].fc20=0.0;
		_node_score_buffer->buffer[_i].roc = 0;
		if(_coverage>0.15f)
		{
			_node_score_buffer->buffer[_i].fc15=_ftimesc;
		}
		if(_coverage>0.20f)
		{
			_node_score_buffer->buffer[_i].fc20=_ftimesc;
			do_roc(&_node_score_buffer->buffer[_i]);
		}
	}
}

void do_roc(node_score *_node_score)
{
	
	if(_node_score->type!=STANDARD)
	{
		return;
	}
	if (_node_score->fc > 1.063f) 
	{
		_node_score->roc=100;
        }
	else if (_node_score->fc > 0.667f)
	{
		_node_score->roc = 75;
        } 
	else if (_node_score->fc > 0.3f) 
	{
		_node_score->roc = 50;
        } else 
	{
        	_node_score->roc = 0;
        }
}
