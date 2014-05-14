#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H
#include "memory_management.h"
#include "string.h"
#include <unicode/utrans.h>
#include <unicode/uenum.h>
#include <unicode/ustring.h>
#include <unicode/ustdio.h>
#include <syslog.h>
#define WORD_POLL_MEMORY_SIZE	PAGE_SIZE*5
#define CHAR_POLL_MEMORY_SIZE	PAGE_SIZE*5
#define NODE_SCORE_POLL_MEMORY_SIZE	PAGE_SIZE*5
#define UCHAR_POLL_MEMORY_SIZE	PAGE_SIZE*6
#define WORD_ID_NOT_FOUND (uint32_t) 0xffffffff
#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
    NODE_TOP = 1, TITLE_MATCH = 2, TITLE_WITH_TOP = 3
} NODE_SCORE_FLAG;
typedef enum {
    STANDARD = 1, DATE = 2, EVENT = 3, PERSON = 4, PLACE = 5, CONCEPT = 6, COMPANY = 7
} NODE_TYPE;
typedef struct
{
	uint32_t node_id;
        uint32_t tax_id;
	float frequency;
    	float fc;
	float fc15;
    	float fc20;
    	float coverage;
    	float frequency_top;
    	float coverage_top;
    	/* This can be this values {0, 50, 55, ,60, 65, 75, 80, 100} */
    	/* ROC stands for "Reciever Operating Characteristics" */
    	uint32_t roc;
    	/*represent state for node top and title match*/
    	NODE_SCORE_FLAG flags; 
    	/*Helps to identify the node type and facilitates the ROC calculation for such node.*/
    	NODE_TYPE type;
	/*memory poll boundary*/
	char padding[16];
}node_score;
typedef struct
{
	memory_page_buffer *page_buffer;
	memory_page_list *page;
	node_score *buffer;
	uint32_t node_score_poll_size;
	uint32_t current_size;	
}node_score_buffer;
typedef struct
{
	memory_page_buffer *buffer;
	memory_page_list *page;	
	uint32_t poll_size;
	uint32_t current_size;
	uint32_t *words;
}word_poll;

typedef struct
{
	memory_page_buffer *page_buffer;
	memory_page_list *page;	
	char *buffer;
	int32_t size;
	uint32_t offset;
	uint32_t capacity;
}char_buffer;

typedef struct
{
	UChar *data;
	int32_t length;
	int32_t capacity;
	memory_page_list *page;	
}u16_char_buffer;

char_buffer *create_char_buffer(memory_page_buffer **_page_buffer);
char_buffer *realloc_char_buffer(char_buffer *_buffer,uint32_t _new_size);
void delete_char_buffer(char_buffer *_buffer);
word_poll *create_word_poll(memory_page_buffer **_page_buffer);
word_poll *realloc_word_poll(word_poll *_word_poll,uint32_t _new_size);
void copy_word_poll(word_poll *_word_poll_source,word_poll *_word_poll_target,uint32_t _size,uint32_t _offset);
void delete_word_poll(word_poll *_word_poll);
/*
 *@Param const *char data utf8 text to convert
 *@Param BUFFER_STATUS current status the buffer creation
 *Return new buffer with text in ut16
 * */
u16_char_buffer *create_u16_char_buffer(memory_page_buffer **_page_buffer);
/*
 * Delete buffer previus created width create_u16_char_buffer
 *@Param u16_char_buffer *buffer delete this buffer and data associated
 * */
void delete_u16_char_buffer(u16_char_buffer *buffer);
/*
 * Create Transformation object this wraper arround transliterate just with id
 *@Param u16_char_buffer *id for create transliterate object actualy we just use the id for transformation
 *Param TRANSFORMATION_STATUS status this flags is the current status about the creation the this transformation object
 *return transformation object
 * */
node_score_buffer *create_node_score_buffer(memory_page_buffer **_buffer);
void delete_node_score_buffer(node_score_buffer *_node_score_buffer);
node_score_buffer *realloc_node_score_buffer(node_score_buffer *_node_score_buffer,uint32_t _new_size);
void clear_word_poll(word_poll *words);
#ifdef __cplusplus
}
#endif


#endif
