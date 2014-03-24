#include "state_aho_poll.h"
static int add_page_and_refixed_array(state_aho_poll *poll);

static int add_page_to_poll(state_aho_poll *poll);

static void calculate_poll_size(state_aho_poll *poll);

state_aho_poll *create_state_aho_poll()
{
	state_aho_poll *poll = (state_aho_poll *)malloc(sizeof(state_aho_poll));
	if(poll == NULL)
	{
		return NULL;
	}
	poll->array = NULL;
	poll->current_page_list = NULL;
	poll->page_list = NULL;
	poll->current_size = 0;
	poll->poll_size = 0;
	poll->all_nodes = 0;	
	return poll;
}

state_aho *create_state_aho_from_poll(state_aho_poll *poll)
{
	uint32_t index = 0;
	if(poll->current_size >= poll->poll_size)
	{
		if(add_page_and_refixed_array(poll) < 0)
		{
			return NULL;
		}
	}
	index = poll->current_size;
	poll->current_size = index + 1;
	poll->all_nodes = poll->all_nodes + 1;
	return &poll->array[index];
}


static int add_page_and_refixed_array(state_aho_poll *poll)
{
	if(add_page_to_poll(poll) < 0)
	{
		return -1;
	}
	calculate_poll_size(poll);
	poll->array =(state_aho *) poll->current_page_list->addr;
	return 0;
}

int add_page_to_poll(state_aho_poll *poll)
{
	memory_page_list *page_list = create_memory_page_list(PAGE_SIZE);
	if(page_list == NULL)
	{
		return -1;
	}
	if(poll->current_page_list != NULL)
	{
		poll->current_page_list->next = page_list;
		poll->current_page_list = page_list;
		return 0;
	}
	poll->current_page_list = page_list;
	poll->page_list = page_list;
	return 0;
}

void calculate_poll_size(state_aho_poll *poll)
{
	 uint32_t poll_size = poll->current_page_list->memory_size / STATE_AHO_SIZE;
	poll->current_size = 0;
	poll->poll_size = poll_size - 1;
}
