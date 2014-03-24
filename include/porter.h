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

typedef struct porter
{
	memory_page_list *page;
	char *buffer;
	size_t size;
	int k,k0,j;
}stemming;
int cons(int i,stemming *_stemming);
int m(stemming *_stemming);
int vowelinstem(stemming *_stemming);
int doublec(int j,stemming *_stemming);
int cvc(int i,stemming *_stemming);
int ends(char * s,stemming *_stemming);
void setto(char * s,stemming *_stemming);
void r(char * s,stemming *_stemming);
void step1ab(stemming *_stemming);
void step1c(stemming *_stemming);
void step2(stemming *_stemming);
void step3(stemming *_stemming);
void step4(stemming *_stemming);
void step5(stemming *_stemming);
int stem(stemming *_stemming);
stemming *build_stemming(memory_page_buffer **_memory_page_buffer);
void delete_stemming(stemming *_stemming);
#ifdef __cplusplus
}
#endif

#endif
