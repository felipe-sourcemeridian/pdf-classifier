#ifndef CLASSIFIER_UTIL_H
#define CLASSIFIER_UTIL_H
#include "memory_poll.h"
#include "state_aho.h"
#include "glib.h"
#include "load_files.h"
#include "syslog.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct
{
	GHashTable *map_title;
	GHashTable *map_summary;
	GHashTable *map_document;
	GHashTable *map_node;
	GHashTable *map_tax_node;
	word_poll *word_poll_document;
	word_poll *word_poll_title;
	word_poll *word_poll_summary;
	node_score_buffer *node_score_buffer;
	uint32_t title_size;
}classifier_document;
typedef struct
{
	GHashTable *map_unfiltered_node_list;
	unfiltered_node_list *unfiltered_node_list;
}unfiltered_node_list_cache;
typedef struct
{
	state_aho_cache *state_aho_cache;
	GHashTable *map_term_node;
	unfiltered_node_list *unfiltered_node_list;
	unfiltered_node_list_cache *unfiltered_node_list_cache;
	GHashTable *map_term_weight_list;
	GHashTable *map_node_must_have;
}classifier;

classifier  *create_classifier(state_aho_cache *_state_aho_cache);
unfiltered_node_list_cache *build_unfiltered_node_list_cache(unfiltered_node_list *_unfiltered_node_list_cache);
void delete_unfiltered_node_list_cache();
void set_map_term_node(classifier *_classifier,GHashTable *_map_term_node);
void set_unfilterd_node(classifier *_classifier,unfiltered_node_list *_unfiltered_node_list);
void set_map_term_weight_list(classifier *_classifier,GHashTable *_map_term_weight_list);
void set_map_node_must_have(classifier *_classifier,GHashTable *_map_node_must_have);
void set_unfiltered_node_list_cache(classifier *_classifier,unfiltered_node_list_cache *_unfiltered_node_list_cache);
void delete_classifier(classifier *_classifier);
classifier_document *create_classifier_document(memory_page_buffer **_buffer);
void delete_classifier_document(classifier_document *_classifier);
void clean_classifier_document(classifier_document *_classifier_document);
int create_classifier_maps(classifier_document *_classifier);
int create_classifier_words_poll(classifier_document *_classifier,memory_page_buffer **_buffer);

#ifdef __cplusplus
}
#endif

#endif
