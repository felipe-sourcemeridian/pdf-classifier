#include "file_transformation.h"
static const char *WORD_DELIMITER = " |.|-|,|:|!|\r|\n|\t|/|\\";

file_transformation *create_file_transformation(memory_page_buffer **_buffer, u16_char_buffer *_id, GHashTable *_hwords) {
    file_transformation *_file_transformation = (file_transformation *) malloc(sizeof (file_transformation));
    stemming *_stemming = NULL;
    transformation *_transformation = NULL;
    u16_char_buffer *_u16_buffer = NULL;
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
    _file_transformation->hwords = _hwords;
    _file_transformation->stemming = _stemming;
    _file_transformation->transformation = _transformation;
    _file_transformation->buffer = _u16_buffer;
    return _file_transformation;
}

void delete_file_transformation(file_transformation *_file_transformation) {
    delete_stemming(_file_transformation->stemming);
    delete_transformation(_file_transformation->transformation);
    delete_u16_char_buffer(_file_transformation->buffer);
    free(_file_transformation);
}

int transform_line(file_transformation *_file_transformation, char_buffer *_buffer) {
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

int word_to_number(file_transformation *file_trans, char_buffer *buffer, word_poll *words) {

    int word_index = words->current_size;
    gpointer original_key;
    gpointer hash_value;
    int size_word_found = 0;
    stemming *stemmer = file_trans->stemming;
    char *word_found = NULL;
    int null_word = 0;
    if (transform_line(file_trans, buffer) < 0) {
        return -1;
    }
    word_found = strtok(buffer->buffer, WORD_DELIMITER);
    while (word_found != NULL) {
        size_word_found = strlen(word_found);
        memcpy(stemmer->buffer, word_found, size_word_found);
        stemmer->k0 = 0;
        stemmer->j = 0;
        stemmer->k = size_word_found - 1;
        null_word = stem(file_trans->stemming) + 1;
        stemmer->buffer[null_word] = 0;
        if (word_index == words->poll_size) {
            realloc_word_poll(words, words->page->memory_size + PAGE_SIZE);
        }

        if (g_hash_table_lookup_extended(file_trans->hwords, file_trans->stemming->buffer, &original_key, &hash_value) == TRUE) {
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
