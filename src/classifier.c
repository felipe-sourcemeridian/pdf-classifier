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
	_unit_workd_time_old=clock();
	copy_word_poll(_word_poll,_classifier_document->word_poll_document,_word_poll->current_size,0);
	copy_word_poll(_word_poll,_classifier_document->word_poll_summary,SUMMARY_SIZE,_classifier_document->title_size);
	copy_word_poll(_word_poll,_classifier_document->word_poll_title,_classifier_document->title_size,0);
	_unit_workd_time_current=clock();
	printf("time for copy words ---->%f\n",(_unit_workd_time_current-_unit_workd_time_old)/CLOCKS_PER_SEC);
/*	printf("************* search phrases title **************\n");*/
	_unit_workd_time_old=clock();
	search_words(_classifier->state_aho_cache,_classifier_document->word_poll_title);
/*	printf("************* search phrases summary **************\n");*/
	search_words(_classifier->state_aho_cache,_classifier_document->word_poll_summary);
/*	printf("************* search phrases document **************\n");*/
	search_words(_classifier->state_aho_cache,_classifier_document->word_poll_document);
	_unit_workd_time_current=clock();
	printf("time for search words ---->%f\n",(float)(_unit_workd_time_current-_unit_workd_time_old)/CLOCKS_PER_SEC);
/*	printf("************* map document **************\n");*/
	_unit_workd_time_old=clock();
	map_words(_classifier_document->map_document,_classifier_document->word_poll_document);
/*	printf("************* map summary **************\n");*/
	map_words(_classifier_document->map_summary,_classifier_document->word_poll_summary);
/*	printf("************* map title **************\n");*/
	map_words(_classifier_document->map_title,_classifier_document->word_poll_title);
	_unit_workd_time_current=clock();
	printf("time for map words ---->%f\n",(float)(_unit_workd_time_current-_unit_workd_time_old)/CLOCKS_PER_SEC);
	_unit_workd_time_old=clock();
/*	printf("************* sort document words **************\n");*/
	sort_word_poll(_classifier_document->word_poll_document);
/*	printf("************* sort document title **************\n");*/
	sort_word_poll(_classifier_document->word_poll_title);
/*	printf("************* sort document summary **************\n");*/
	sort_word_poll(_classifier_document->word_poll_summary);
	_unit_workd_time_current=clock();
	printf("time for sort words ---->%f\n",(float)(_unit_workd_time_current-_unit_workd_time_old)/CLOCKS_PER_SEC);
	_unit_workd_time_old=clock();
/*	printf("************* search node by terms **************\n");*/
	search_node_by_terms(_classifier,_classifier_document);
	printf("map node size %d\n",g_hash_table_size(_classifier_document->map_node));
	printf("map node tax size %d\n",g_hash_table_size(_classifier_document->map_tax_node));
	_unit_workd_time_current=clock();
	printf("time search node by terms ---->%f\n",(float)(_unit_workd_time_current-_unit_workd_time_old)/CLOCKS_PER_SEC);
	_unit_workd_time_old=clock();
/*	printf("************* intersection nodes **************\n");*/
	intersection_nodes(_classifier,_classifier_document);
	_unit_workd_time_current=clock();
	printf("time for intersection node ---->%f\n",(float)(_unit_workd_time_current-_unit_workd_time_old)/CLOCKS_PER_SEC);
	_unit_workd_time_old=clock();
/*	printf("************* node for score size %d ***********\n",_classifier_document->node_score_buffer->current_size);*/
/*	printf("************* score classfier document ************\n");*/
	score_classifier_document(_classifier,_classifier_document);
	_unit_workd_time_current=clock();
	printf("time for score classifier document ---->%f\n",(float)(_unit_workd_time_current-_unit_workd_time_old)/CLOCKS_PER_SEC);
	_unit_workd_time_old=clock();
/*	printf("************* do score binary title **************\n");*/
	do_score_binary(_classifier_document->word_poll_title,_classifier,_classifier_document,TITLE_MATCH);
/*	printf("************* do score binary summary **************\n");*/
	do_score_binary(_classifier_document->word_poll_summary,_classifier,_classifier_document,NODE_TOP);
	_unit_workd_time_current=clock();
	printf("time for score binary ---->%f\n",(float)(_unit_workd_time_current-_unit_workd_time_old)/CLOCKS_PER_SEC);
	_unit_workd_time_old=clock();
/*	printf("************* do combine score **************\n");*/
	do_combine_score(_classifier_document);
	_unit_workd_time_current=clock();
	printf("time for combine score ---->%f\n",(float)(_unit_workd_time_current-_unit_workd_time_old)/CLOCKS_PER_SEC);
	_classify_time_current=clock();
	printf("time for classifier document ---->%f\n",(float)(_classify_time_current-_classify_time_old)/CLOCKS_PER_SEC);
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
