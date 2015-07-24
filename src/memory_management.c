#include "memory_management.h"
#include "time.h"
#include "stdio.h"

int file_size(int _fd) {
    struct stat _stat;
    if (fstat(_fd, &_stat) < 0) {
        return -1;
    }
    return _stat.st_size;
}

void *map_file(const char *_file_name, memory_page_buffer **_buffer, unsigned char _write) {
    memory_page_list *_page = NULL;
    int _file_size = -1;
    int _fd = -1;
    int _flags = O_RDWR;
    int _prot = PROT_READ;
    if (_write == 0) {
        _flags = O_RDONLY;
        _prot = _prot | PROT_WRITE;
    }
    _fd = open(_file_name, _flags);
    if (_fd < 0) {
        return NULL;
    }
    _file_size = file_size(_fd);
    if (_file_size < 0) {
        close(_fd);
        return NULL;
    }
    _page = create_memory_page_list(_file_size, _prot, MAP_PRIVATE, _fd);
    close(_fd);
    if (_page == NULL) {
        return NULL;
    }
    *_buffer = add_page_to_buffer(*_buffer, _page);
    return _page->addr;
}

memory_page_buffer *add_page_to_buffer(memory_page_buffer *_buffer, memory_page_list *_page) {
    memory_page_buffer *_new_buffer = _buffer;
    if (_new_buffer) {
        _new_buffer->page_list_size = _new_buffer->page_list_size + 1;
        _new_buffer->memory_size = _new_buffer->memory_size + _page->memory_size;
        _page->next = _new_buffer->page;
        _new_buffer->page = _page;
    } else {
        _new_buffer = (memory_page_buffer *) malloc(sizeof (memory_page_buffer));
        if (!_new_buffer) {
            return NULL;
        }
        _new_buffer->page = _page;
        _new_buffer->page_list_size = 1;
        _new_buffer->memory_size = _page->memory_size;
    }
    return _new_buffer;
}

void free_memory_page_buffer(memory_page_buffer *_buffer) {
    memory_page_list *_page_current = _buffer->page;
    memory_page_list *_page_next = _page_current;
    while (_page_next) {
        _page_next = _page_current->next;
        munmap(_page_current->addr, _page_current->memory_size);
        free(_page_current);
        _page_current = _page_next;
    }
    free(_buffer);
}

void free_front_page_from_buffer(memory_page_buffer *_buffer) {
    memory_page_list *_memory_page_list = _buffer->page;
    _buffer->page = _memory_page_list->next;
    _buffer->memory_size = _buffer->memory_size - _memory_page_list->memory_size;
    munmap(_memory_page_list->addr, _memory_page_list->memory_size);
    free(_memory_page_list);
}

memory_page_list *create_page() {
    memory_page_list *_memory_page_list = create_memory_page_list(PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0);
    return _memory_page_list;
}

memory_page_list *create_memory_page_list(unsigned int _page_size, unsigned int _prot, unsigned int _flags, int _fd) {
    memory_page_list *_memory_page_list = NULL;
    void *_page_memory = NULL;
    _page_memory = mmap(NULL, _page_size, _prot, _flags, _fd, 0);
    if (_page_memory == MAP_FAILED) {
        return NULL;
    }
    _memory_page_list = (memory_page_list *) malloc(sizeof (memory_page_list));
    if (_memory_page_list == NULL) {
        munmap(_page_memory, _page_size);
        return NULL;
    }
#ifndef CLASSIFIER_SWAPON
    if (mlock(_page_memory, _page_size) < 0) {
        munmap(_page_memory, _page_size);
        free(_memory_page_list);
        return NULL;
    }
#endif
    _memory_page_list->memory_size = _page_size;
    _memory_page_list->addr = _page_memory;
    _memory_page_list->next = NULL;
    return _memory_page_list;
}

memory_page_list* realloc_memory_page(memory_page_list *_memory_page_list, unsigned int _new_page_size) {
    void *_old_memory_page = _memory_page_list->addr;
    void *_new_page = NULL;
    unsigned int _old_memory_size = _memory_page_list->memory_size;
    munlock(_old_memory_page, _old_memory_size);
    _new_page = mremap(_old_memory_page, _old_memory_size, _new_page_size, MREMAP_MAYMOVE);
    printf("************** memory map  realloc size %d***************\n", _memory_page_list->memory_size);
    if (_new_page == MAP_FAILED) {
        return NULL;
    }
#ifndef CLASSIFIER_SWAPON
    mlock(_new_page, _new_page_size);
#endif
    _memory_page_list->addr = _new_page;
    _memory_page_list->memory_size = _new_page_size;
    return _memory_page_list;
}
