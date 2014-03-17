#ifndef SCORE_DOCUMENT_H
#define SCORE_DOCUMENT
#include "memory_poll.h"
#include "glib.h"
#include "load_files.h"
#include "classifier_utils.h"
#include "string.h"

#ifdef __cplusplus
extern "C" {
#endif

void map_node(GHashTable *_hnode,uint32_t *_words,uint32_t _size);
int search_node_by_terms(classifier *_classifier,classifier_document *_classifier_document);
int intersection_nodes(classifier *_classifier,classifier_document *_classifier_document);
void do_score(term_weight_list *_term_weight_list,word_poll *_word_poll,GHashTable *_map,float *_frequency,float *_coverega);
void do_score_binary(word_poll *_word_poll,classifier *_classifier,classifier_document *_classifier_document,NODE_SCORE_FLAG flags);
void do_combine_score(classifier_document *_classifier_document);
void do_roc(node_score *_node_score);
int score_classifier_document(classifier *_classifier,classifier_document *_classifier_document);

#ifdef __cplusplus
}
#endif

#endif
