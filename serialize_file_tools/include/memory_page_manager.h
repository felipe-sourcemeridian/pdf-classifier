#ifndef MEMORY_PAGE_MANAGER
#define MEMORY_PAGE_MANAGER
#define _GNU_SOURCE
#include <stdint.h>
#include "unistd.h"
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#define PAGE_SIZE getpagesize()
typedef struct _memory_page_list
{
	void *addr;
	uint32_t memory_size;
	struct _memory_page_list *next;
}memory_page_list;
memory_page_list *create_memory_page_list(unsigned int pages);
#endif
