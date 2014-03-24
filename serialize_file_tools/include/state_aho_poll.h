#ifndef STATE_AHO_POLL
#define STATE_AHO_POLL
#ifdef __cplusplus
extern "C" {
#endif
#include "inttypes.h"
#include "state_aho_def.h"
#include "memory_page_manager.h"
#define STATE_AHO_PAGES	39523
#define STATE_AHO_SIZE sizeof(state_aho)
typedef struct
{
	memory_page_list *page_list;
	memory_page_list *current_page_list;
	state_aho *array;
	uint32_t all_nodes;
	uint32_t current_size;
	uint32_t poll_size;
}state_aho_poll;
state_aho_poll *create_state_aho_poll();
state_aho *create_state_aho_from_poll(state_aho_poll *poll);

#ifdef __cplusplus
}
#endif

#endif
