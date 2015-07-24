#include "memory_page_manager.h"
memory_page_list *create_memory_page_list(unsigned int pages)
{
	memory_page_list *page_list = NULL;
	int memory_protection = PROT_READ | PROT_WRITE;
	int memory_flags = MAP_PRIVATE | MAP_ANONYMOUS;
	void *addr_start = NULL;
	void *memory_addr = NULL;
	int fd = 0;
	off_t offset = 0;
	size_t page_size = PAGE_SIZE * pages;
	memory_addr = mmap(addr_start, page_size, memory_protection, memory_flags, fd, offset);
	if(memory_addr == MAP_FAILED)
	{
		return NULL;
	}
	page_list = (memory_page_list *)malloc(sizeof(memory_page_list));
	if(page_list == NULL)
	{
		munmap(memory_addr, page_size);
		return NULL;
	}
        #ifndef CLASSIFIER_SWAPON
	if(mlock(memory_addr, page_size) < 0 )
	{
		munmap(memory_addr, page_size);
		free(page_list);
		return NULL;	
	}
        #endif
	page_list->memory_size = page_size;
	page_list->addr = memory_addr;
	page_list->next = NULL;
	return page_list;
}
