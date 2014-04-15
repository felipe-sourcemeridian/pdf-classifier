#include "classifier.h"
void sort_word_poll(word_poll *_word_poll)
{
	int _size=_word_poll->current_size;
	uint32_t *_words=_word_poll->words;
	uint32_t _aux_word=0;
	uint32_t _i=0;
	uint32_t _j=0;
	for(_i=0;_i<_size;_i++)
	{	
		for(_j=(_i+1);_j<_size;_j++)
		{
			if(_words[_i]>_words[_j])
			{
				_aux_word=_words[_i];
				_words[_i]=_words[_j];
				_words[_j]=_aux_word;
			}
		}
	}
}

int classify(classifier *_classifier,classifier_document *_classifier_document,word_poll *_word_poll)
{
	clock_t _classify_time_old=clock();
	clock_t _classify_time_current=clock();
	clock_t _unit_workd_time_old;
	clock_t _unit_workd_time_current;
	if(_word_poll->poll_size!=_classifier_document->word_poll_document->poll_size)
	{
		realloc_word_poll(_classifier_document->word_poll_document,_word_poll->page->memory_size);
	}
	if(_word_poll->current_size==0)
	{
		return 0;
	}
	_classifier_document->document_size = _word_poll->current_size; 
	copy_word_poll(_word_poll,_classifier_document->word_poll_document,_word_poll->current_size,0);

	copy_word_poll(_word_poll,_classifier_document->word_poll_summary,SUMMARY_SIZE,_classifier_document->title_size);

	copy_word_poll(_word_poll,_classifier_document->word_poll_title,_classifier_document->title_size,0);

	search_words(_classifier->state_aho_cache,_classifier_document->word_poll_title);

	search_words(_classifier->state_aho_cache,_classifier_document->word_poll_summary);

	search_words(_classifier->state_aho_cache,_classifier_document->word_poll_document);

	map_words(_classifier_document->map_document,_classifier_document->word_poll_document);

	map_words(_classifier_document->map_summary,_classifier_document->word_poll_summary);

	map_words(_classifier_document->map_title,_classifier_document->word_poll_title);

	sort_word_poll(_classifier_document->word_poll_document);
	sort_word_poll(_classifier_document->word_poll_title);
	sort_word_poll(_classifier_document->word_poll_summary);

	search_node_by_terms(_classifier,_classifier_document);

	intersection_nodes(_classifier,_classifier_document);

	score_classifier_document(_classifier,_classifier_document);

	do_score_binary(_classifier_document->word_poll_title,_classifier,_classifier_document,TITLE_MATCH);

	do_score_binary(_classifier_document->word_poll_summary,_classifier,_classifier_document,NODE_TOP);

	do_combine_score(_classifier_document);
	return 0;
}
void map_words(GHashTable *_map,word_poll *_word_poll)
{
	uint32_t _current_size=_word_poll->current_size;	
	uint32_t _i=0;
	uint32_t _count=0;
	uint32_t _word=0;
	gpointer _value=NULL;
	gpointer _original_key=NULL;
	for(;_i<_current_size;_i++)
	{
		_word=_word_poll->words[_i];
		if(g_hash_table_lookup_extended(_map,GINT_TO_POINTER(_word),&_original_key,&_value)==FALSE)
		{
			_value=GINT_TO_POINTER(1);
		}else
		{
			_count=GPOINTER_TO_INT(_value)+1;
			_value=GINT_TO_POINTER(_count);
		}
		_original_key=GINT_TO_POINTER(_word);
		g_hash_table_insert(_map,_original_key,_value);
	}
}
