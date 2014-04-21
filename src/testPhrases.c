#include "trans.h"
#include "state_aho.h"
#include "memory_poll.h"
#include "stdlib.h"
#include "stdio.h"
#include "load_files.h"
#include "file_transformation.h"
#include "utility.h"

#define WORD_TEST "She was a U.S. Fed Cup team coach in 2006 and 2008."
static const char *WORD_DELIMITER = " ";
static const char * STATE_AHO_FILE_NAME = "/usr/local/classifier_static_data/state.aho";
static const char *WORDS_FILE_NAME = "/usr/local/classifier_static_data/words.data";
static const char *REMOVE_ACCENTS = "NFD;[:Nonspacing Mark:] Remove;NFC;Lower";
static const char *FILE_NAME = "/home/carlos/new development machine/new  2.txt";
/*static const char *FILE_NAME = "/home/carlos/new development machine/test2.txt";*/
static const char *PHRASES_ID_FILE_NAME = "/home/carlos/new development machine/phrases_id_c.txt";

static void transform_file_and_save_phrases_id(const char *file_name, const char * phrases_id_file, file_transformation *file_trans, state_aho_cache *phrases_tree, word_poll *words, char_buffer *buffer);
static void read_and_transform_file(const char *file_name, file_transformation *file_trans, char_buffer * buffer, word_poll *words);
static void find_phrases_and_save_file(const char *phrases_id_file_name, state_aho_cache *phrases_tree, word_poll *words);
static void test_split_words();

int main(int argc, char **argv) {
    memory_page_buffer *page_buffer = NULL;
    word_poll *words = create_word_poll(&page_buffer);
    state_aho_cache *phrases_tree = load_state_aho(STATE_AHO_FILE_NAME, &page_buffer);
    GHashTable *hwords = load_words(WORDS_FILE_NAME, &page_buffer);
    file_transformation *file_trans = NULL;
    u16_char_buffer *id = create_id(REMOVE_ACCENTS);
    char_buffer *buffer = create_char_buffer(&page_buffer);
    if (hwords == NULL) {
        perror("error to load words mapping\n");
        exit(-1);
    }
    if (buffer == NULL) {
        perror("error to create char buffer\n");
        exit(-1);
    }
    if (words == NULL) {
        perror("error to create poll\n");
        exit(-1);
    }
    if (phrases_tree == NULL) {
        perror("error to build tree\n");
        exit(-1);
    }
    if (id == NULL) {
        perror("can not build transliterator id\n");
        exit(-1);
    }
    file_trans = create_file_transformation(&page_buffer, id, hwords);
    if (file_trans == NULL) {
        perror("can not build file_transformation\n");
        exit(-1);
    }
    transform_file_and_save_phrases_id(FILE_NAME, PHRASES_ID_FILE_NAME, file_trans, phrases_tree, words, buffer);
    /*test_split_words();*/
    return 0;
}

static void transform_file_and_save_phrases_id(const char *file_name, const char * phrases_id_file, file_transformation *file_trans, state_aho_cache *phrases_tree, word_poll *words, char_buffer *buffer) {
    read_and_transform_file(file_name, file_trans, buffer, words);
    find_phrases_and_save_file(phrases_id_file, phrases_tree, words);
}

static void test_split_words() {
    const char *test = "Sofia  Carlos Viera";
    char *data = (char *) malloc(400);
    char *word_found = NULL;
    int length_test = strlen(test);
    memcpy(data, test, length_test);
    data[length_test] = 0;
    printf("%s\n", data);
    word_found = strtok(data, WORD_DELIMITER);
    while (word_found != NULL) {
        printf("word found %s\n", word_found);
        word_found = strtok(NULL, WORD_DELIMITER);
    }
}

static void read_and_transform_file(const char *file_name, file_transformation *file_trans, char_buffer * buffer, word_poll *words) {
    FILE *text_file = fopen(file_name, "r");
    char *raw_data = buffer->buffer;
    uint32_t buffer_size = buffer->capacity;
    size_t byte_reads = 0;
    if (text_file == NULL) {
        perror("file not found\n");
        exit(-1);
    }
    while ((byte_reads = getline(&raw_data, &buffer_size, text_file)) != -1) {
        buffer_size = buffer->capacity;
        buffer->size = byte_reads;
        word_to_number(file_trans, buffer, words);
    }
    fclose(text_file);
}

static void find_phrases_and_save_file(const char *phrases_id_file_name, state_aho_cache *phrases_tree, word_poll *words) {
    FILE *phrases_id_file = fopen(phrases_id_file_name, "w");
    uint32_t i = words->current_size;
    if (phrases_id_file == NULL) {
        perror("error to open file to write \n");
        exit(-1);
    }
    search_words(phrases_tree, words);
    for (; i < words->current_size; i++) {
        fprintf(phrases_id_file, "%d\n", words->words[i]);
    }
    fclose(phrases_id_file);
}
