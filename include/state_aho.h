#ifndef STATE_AHO_H
#define STATE_AHO_H
#include <stdio.h>
#include "stdlib.h"
#include "state_aho_def.h"
#include "inttypes.h"
#include "memory_poll.h"
#include "glib.h"
/*
 *State machine Aho-Corasick
 *Multipattern Search
 * */
#ifdef __cplusplus
extern "C" {
#endif

/*
 *State Machine Aho-Corasick
 *Improve the Search
 * */
struct _state_aho_cache
{
	struct _state_aho *state_aho;
	GHashTable *cache;	
};

typedef struct _state_aho_cache state_aho_cache;
state_aho_cache *load_state_aho(const char *_file_name, memory_page_buffer **_buffer);

void delete_state_aho_cache(state_aho_cache *_state_aho_cache);

void build_cache(state_aho_cache *_state_aho_cache);

struct _state_aho *find_node_transition_by_node_fail(state_aho_cache *_state_aho_cache, struct _state_aho *_node_fail, uint32_t _word_id);

void fixed_pointers(void *_base_pointer, uint64_t _offset);

void search_words(state_aho_cache *_state_aho_cache, word_poll *_word_poll);

state_aho *get_transitions(uint32_t _word_id,state_aho *_current_node,state_aho_cache *_state_aho_cache);

#ifdef __cplusplus
}
#endif

#endif
