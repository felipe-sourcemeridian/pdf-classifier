#ifndef FILE_TRANSFORMATION_H
#define FILE_TRANSFORMATION_H
#include "trans.h"
#include "porter.h"
#include "glib.h"
#include "memory_poll.h"
#include "unistd.h"
#include "string.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	struct stemmer *stemming;
	struct char_transformation *transformation;
	GHashTable *hwords;
	u16_char_buffer *buffer;	
}file_transformation;

file_transformation *create_file_transformation(memory_page_buffer **_buffer, u16_char_buffer *_id, GHashTable *_hwords);

void delete_file_transformation(file_transformation *_file_transformation);

int word_to_number(file_transformation *_file_transformation, char_buffer *_buffer, word_poll *_word_poll);

int transform_line(file_transformation *_file_transformation, char_buffer *_buffer);

#ifdef __cplusplus
}
#endif

#endif
 
