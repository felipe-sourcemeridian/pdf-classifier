#ifndef STATE_AHO_H
#define STATE_AHO_H
#include <stdio.h>
#include "stdlib.h"
#include "inttypes.h"
#include "memory_poll.h"
#include "glib.h"
/*
 *State machine Aho-Corasick
 *Multipattern Search
 * */
typedef struct _state_aho
{
	 struct _state_aho *parent;
	 uint32_t word_id;
	 uint32_t value;
	 struct _state_aho *fail;
	 uint32_t size;
	 unsigned char has_result;
	 unsigned char is_null_node;
	 char padding[2];
	 struct _state_aho *childs[0];
}state_aho;
/*
 *State Machine Aho-Corasick
 *Improve the Search
 * */
typedef struct 
{
	state_aho *state_aho;
	GHashTable *cache;	
}state_aho_cache;

state_aho_cache *load_state_aho(const char *_file_name,memory_page_buffer **_buffer);

void delete_state_aho_cache(state_aho_cache *_state_aho_cache);

void build_cache(state_aho_cache *_state_aho_cache);

state_aho *find_node_transition_by_node_fail(state_aho_cache *_state_aho_cache,state_aho *_node_fail,uint32_t _word_id);

void fixed_pointers(void *_base_pointer,uint64_t _offset);

void search_words(state_aho_cache *_state_aho_cache,word_poll *_word_poll);

state_aho *get_transitions(uint32_t _word_id,state_aho *_current_node,state_aho_cache *_state_aho_cache);
#endif
