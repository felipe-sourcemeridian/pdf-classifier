#ifndef UTILITY_H
#define  UTILITY_H
#include "stdlib.h"
#include "stdio.h"
#include "stdint.h"
#include "memory_poll.h"
#include "state_aho.h"
#include "classifier.h"
#include "config_util.h"
u16_char_buffer *create_id(const char *_id);
void delete_id(u16_char_buffer *_buffer);
void delete_id(u16_char_buffer *_buffer);
classifier *build_classifier(GKeyFile *_config_file,memory_page_buffer **_memory_page_buffer);
#endif
