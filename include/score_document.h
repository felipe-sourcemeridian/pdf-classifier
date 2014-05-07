#ifndef SCORE_DOCUMENT_H
#define SCORE_DOCUMENT
#include "memory_poll.h"
#include "glib.h"
#include "load_files.h"
#include "classifier_utils.h"
#include "string.h"
#define SCORE_100   100
#define SCORE_80   80
#define SCORE_75   75
#define SCORE_65   65
#define SCORE_60   60
#define SCORE_55   55
#define SCORE_50   50
#define IS_SCORE_COVERAGE_GT_20(score) score->coverage > 0.20f
#define IS_TITLE_MATCH(score) (score->flags & TITLE_MATCH) != 0
#define IS_SCORE_100(score)  IS_SCORE_COVERAGE_GT_20(score)  &&  score->fc > 1.063f
#define IS_SCORE_75(score)   IS_SCORE_COVERAGE_GT_20(score)  &&  score->fc > 0.667f
#define IS_SCORE_80(score)   IS_SCORE_75(score) && IS_TITLE_MATCH(score)
#define IS_SCORE_50(score)   IS_SCORE_COVERAGE_GT_20(score)  &&  score->fc > 0.3f
#define IS_SCORE_55(score)   score->fc > 0.3f  &&  IS_TITLE_MATCH(score)
#define IS_SCORE_60(score)   score->coverage > 0.125f && score->fc > 0.6f
#define IS_SCORE_65(score)   IS_SCORE_60(score) && IS_TITLE_MATCH(score)
#ifdef __cplusplus
extern "C" {
#endif

    void map_node(GHashTable *_hnode, uint32_t *_words, uint32_t _size);
    int search_node_by_terms(classifier *_classifier, classifier_document *_classifier_document);
    int intersection_nodes(classifier *_classifier, classifier_document *_classifier_document);
    void do_score(term_weight_list *_term_weight_list, word_poll *_word_poll, GHashTable *_map, float *_frequency, float *_coverega, uint32_t document_size);
    void do_score_binary(word_poll *_word_poll, classifier *_classifier, classifier_document *_classifier_document, NODE_SCORE_FLAG flags);
    void do_combine_score(classifier_document *_classifier_document);    
    int score_classifier_document(classifier *_classifier, classifier_document *_classifier_document);

#ifdef __cplusplus
}
#endif

#endif
