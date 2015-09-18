#ifndef FILE_TRANSFORMATION_TEST
#define FILE_TRANSFORMATION_TEST
#include "file_transformation.h"
#include "load_files.h"
#include "TestHarness.h"
word_struct *create_word(const char *word_char, int id);
file_transformation *create_file_transformation_from_id(const char *char_id);
u16_char_buffer *create_id(const char *_id);	
#endif
