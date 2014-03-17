#ifndef CLASSIFIER_H
#define CLASSIFIER_H
#include "classifier_utils.h"
#include "score_document.h"
#include "time.h"
#ifndef SUMMARY_SIZE
#define SUMMARY_SIZE 100
#endif
#ifdef __cplusplus
extern "C" {
#endif
int classify(classifier *_classifier,classifier_document *_classifier_document,word_poll *_word_poll);
void sort_word_poll(word_poll *_word_poll);
void map_words(GHashTable *_map, word_poll *_word_poll);

#ifdef __cplusplus
}
#endif

#endif
