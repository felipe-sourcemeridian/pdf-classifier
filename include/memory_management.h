#ifndef MEMORY_MANAGEMENT_H
#define MEMORY_MANAGEMENT_H
#define _GNU_SOURCE
#include <stdint.h>
#include "unistd.h"
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#ifndef PAGE_SIZE
#define PAGE_SIZE getpagesize()
#endif
#ifdef	__cplusplus
extern "C" {
#endif

typedef struct _memory_page_list {
        void *addr;
        uint32_t memory_size;
        struct _memory_page_list *next;
    } memory_page_list;

    typedef struct _memory_page_buffer {
        memory_page_list *page;
        uint32_t page_list_size;
        uint32_t memory_size;
    } memory_page_buffer;

    memory_page_buffer *add_page_to_buffer(memory_page_buffer *_buffer, memory_page_list *_page);

    void free_front_page_from_buffer(memory_page_buffer *_buffer);

    /*size page aligned*/
    memory_page_list* realloc_memory_page(memory_page_list *_memory_page_list, unsigned int _new_page_size);

    memory_page_list *create_memory_page_list(unsigned int _page_size, unsigned int _prot, unsigned int _flags, int _fd);

    memory_page_list *create_page();

    void *map_file(const char *_file_name, memory_page_buffer **_buffer, unsigned char _write);

    void free_memory_page_buffer(memory_page_buffer *_buffer);

    int file_size(int _fd);

#ifdef	__cplusplus
}
#endif

#endif
