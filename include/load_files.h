#ifndef LOAD_WORDS_H
#define LOAD_WORDS_H
#include "glib.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "memory_management.h"
#define WORD_STRUCT_SIZE sizeof(word_struct)
typedef struct 
{
	uint32_t word_id;
	unsigned char size;
	char word[0];
}word_struct;
typedef struct
{
	uint32_t term_id;
	float weight;
}term_weight;
typedef struct
{
	uint32_t term_id;
	uint32_t size_code_list_node;
	uint32_t size_code_list_tax;
	uint32_t code_list_buffer[0];	
}term_nodes;
typedef struct
{
	unsigned char type;
	char padding[3];/*manual padding force alignment change in specefid arch*/
	uint32_t node_id;
	uint32_t size;		
	term_weight term_weight_buffer[0];
	char padding2[4];	
}term_weight_list;
typedef struct
{
	uint32_t node_id;
	uint32_t size;
	uint32_t words[0];
}node_musthave_term;
typedef struct
{
	uint32_t size;
	uint32_t nodes[0];
}unfiltered_node_list;
GHashTable *make_hash(const char *_file_name,memory_page_buffer **_buffer,GHashFunc _hashFunc,GEqualFunc _equalFunc,void **_data);
GHashTable *load_node_sig_term(const char *_file_name,memory_page_buffer **_buffer);
GHashTable *load_words(const char *_file_name,memory_page_buffer **_buffer);
GHashTable *load_list_node(const char *_file_name,memory_page_buffer **_buffer);
GHashTable *load_node_musthave_term(const char *_file_name,memory_page_buffer **_buffer);
unfiltered_node_list *load_unfiltered_nodes(const char *_file_name,memory_page_buffer**_buffer);
#endif
