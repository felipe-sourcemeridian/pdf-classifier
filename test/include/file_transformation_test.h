#ifndef FILE_TRANSFORMATION_TEST
#define FILE_TRANSFORMATION_TEST
#include "TestHarness.h"
#include "file_transformation.h"
#include "utility.h"
word_struct *create_word(const char *word_char, int id);
file_transformation *create_file_transformation_from_id(const char *char_id);
#endif
