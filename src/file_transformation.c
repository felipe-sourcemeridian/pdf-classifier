#include "file_transformation.h"
static const char *WORD_DELIMITER = " |-|,|:|!|\r|\n|\t|/|\\";
static const char ACRONYM_POINT = '.';
static int is_word_acronym(char *word);
static int transform_word(file_transformation *_file_transformation, char_buffer *_buffer);
static void remove_dots(char_buffer *buffer);
static char get_next_valid_char(char *buffer, uint32_t index, uint32_t size);
file_transformation *create_file_transformation(memory_page_buffer **_buffer, u16_char_buffer *_id, GHashTable *_hwords) {
    file_transformation *_file_transformation = (file_transformation *) malloc(sizeof (file_transformation));
    stemming *_stemming = NULL;
    transformation *_transformation = NULL;
    u16_char_buffer *_u16_buffer = NULL;
    char_buffer *word_buffer = NULL;
    TRANSFORMATION_STATUS _status;
    if (_file_transformation == NULL) {
        return NULL;
    }
    _stemming = build_stemming(_buffer);
    if (_stemming == NULL) {
        free(_file_transformation);
        return NULL;
    }
    _transformation = create_transformation(_id, &_status);
    if (_transformation == NULL) {
        delete_stemming(_stemming);
        free(_file_transformation);
        return NULL;
    }
    _u16_buffer = create_u16_char_buffer(_buffer);
    if (_u16_buffer == NULL) {
        delete_stemming(_stemming);
        delete_transformation(_transformation);
        free(_file_transformation);
        return NULL;
    }
    word_buffer = create_char_buffer(_buffer);
    if (word_buffer == NULL) {
        delete_stemming(_stemming);
        delete_transformation(_transformation);
        delete_u16_char_buffer(_u16_buffer);
        free(_file_transformation);
        return NULL;
    }
    _file_transformation->hwords = _hwords;
    _file_transformation->stemming = _stemming;
    _file_transformation->transformation = _transformation;
    _file_transformation->buffer = _u16_buffer;
    _file_transformation->word_buffer = word_buffer;
    return _file_transformation;
}

void delete_file_transformation(file_transformation *_file_transformation) {
    delete_stemming(_file_transformation->stemming);
    delete_transformation(_file_transformation->transformation);
    delete_u16_char_buffer(_file_transformation->buffer);
    free(_file_transformation);
}

int word_to_number(file_transformation *file_trans, char_buffer *buffer, word_poll *words) {

    int word_index = words->current_size;
    gpointer original_key;
    gpointer hash_value;
    int word_size = 0;
    char *word_found = NULL;
    word_found = strtok(buffer->buffer, WORD_DELIMITER);
    while (word_found != NULL) {
        word_size = strlen(word_found) + 1;
        memcpy(file_trans->word_buffer->buffer, word_found, word_size);
        file_trans->word_buffer->size = word_size;
        if (!is_word_acronym(word_found)) {
            if (transform_word(file_trans, file_trans->word_buffer) < 0) {
                return -1;
            }            
        }
        remove_dots(file_trans->word_buffer);
        if (word_index == words->poll_size) {
            realloc_word_poll(words, words->page->memory_size + PAGE_SIZE);
        }

        if (g_hash_table_lookup_extended(file_trans->hwords, file_trans->word_buffer->buffer, &original_key, &hash_value) == TRUE) {
            words->words[word_index] = GPOINTER_TO_INT(hash_value);
        } else {
            words->words[word_index] = WORD_ID_NOT_FOUND;
        }

        words->current_size = words->current_size + 1;
        word_index = word_index + 1;
        word_found = strtok(NULL, WORD_DELIMITER);
    }
    if (word_index == words->poll_size) {
        realloc_word_poll(words, words->page->memory_size + PAGE_SIZE);
    }
    words->current_size = words->current_size + 1;
    words->words[word_index] = WORD_ID_NOT_FOUND;
    return 0;
}

static int transform_word(file_transformation *_file_transformation, char_buffer *_buffer) {
    UErrorCode error;
    TRANSFORMATION_STATUS status;
    u16_char_buffer *_u16_buffer = _file_transformation->buffer;
    _u16_buffer->length = 0;
    memset(&error, 0, sizeof (UErrorCode));
    memset(&status, 0, sizeof (TRANSFORMATION_STATUS));
    u_strFromUTF8WithSub(_u16_buffer->data, _u16_buffer->capacity, &_u16_buffer->length, _buffer->buffer, _buffer->size, 0xFFFD, NULL, &error);
    if (U_FAILURE(error)) {
        return -1;
    }
    transform(_file_transformation->transformation, _u16_buffer, &status);
    if (status != TRANSFORMATION_OK) {
        return -1;
    }

    u_strToUTF8WithSub(_buffer->buffer, _buffer->capacity, &_buffer->size, _u16_buffer->data, _u16_buffer->length, 0xFFFD, NULL, &error);
    if (U_FAILURE(error)) {
        return -1;
    }
    return 0;
}

static void remove_dots(char_buffer *buffer)
{
    uint32_t i;
    uint32_t size = buffer->size;
    uint32_t index = 0;
    char char_valid = 0;
    for(i =0;i<size;i++)
    {
      if(buffer->buffer[i] != ACRONYM_POINT)
      {
          index++;
          continue;
      }
      char_valid = get_next_valid_char(buffer->buffer, i, size);
      if(char_valid == 0)
      {
          break;
      }
      buffer->buffer[i] = char_valid;
      index++;
    }
    buffer->buffer[index] = 0;
    buffer->size = index + 1;
    
}

static char get_next_valid_char(char *buffer, uint32_t index, uint32_t size)
{
    uint32_t i;
    char valid_char = 0;
    for(i=index;i<size;i++)
    {
        if(buffer[i] != ACRONYM_POINT)
        {
            valid_char = buffer[i];
            buffer[i]= ACRONYM_POINT;
            return valid_char;
        }
    }
    return 0;
}

static int is_word_acronym(char *word) {
    int word_length = strlen(word);
    int i;
    for (i = 0; i < word_length; i++) {
        if (ACRONYM_POINT == word[i] || isupper(word[i]) || isdigit(word[i])) {
            continue;
        }
        return 0;
    }
    return 1;
}