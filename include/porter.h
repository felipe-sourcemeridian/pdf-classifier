#ifndef PORTER_H
#define PORTER_H
#include <stdio.h>
#include <string.h>
#include "memory_management.h"
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct stemmer
{
	memory_page_list *page;
        size_t size;
	char *b;	
	int k;
        int j;
}stemming;
int stem(struct stemmer *_stemming);
struct stemmer *build_stemming(memory_page_buffer **_memory_page_buffer);
void delete_stemming(stemming *porter);

#ifdef __cplusplus
}
#endif

#endif
