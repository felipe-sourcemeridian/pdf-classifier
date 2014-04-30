#include "classifier_utils.h"

classifier *create_classifier(state_aho_cache *_state_aho_cache) {
    classifier *_classifier = (classifier *) malloc(sizeof (classifier));
    if (_classifier == NULL) {
        return NULL;
    }
    _classifier->state_aho_cache = _state_aho_cache;
    _classifier->map_term_node = NULL;
    _classifier->unfiltered_node_list = NULL;
    _classifier->map_term_weight_list = NULL;
    _classifier->map_node_must_have = NULL;
    return _classifier;
}

unfiltered_node_list_cache *build_unfiltered_node_list_cache(unfiltered_node_list *_unfiltered_node_list) {
    unfiltered_node_list_cache *_unfiltered_node_list_cache = (unfiltered_node_list_cache *) malloc(sizeof (unfiltered_node_list_cache));
    GHashTable *_map_unfiltered_node_list = NULL;
    int _i;
    uint32_t _node_id;
    if (_unfiltered_node_list_cache == NULL) {
        return NULL;
    }
    _map_unfiltered_node_list = g_hash_table_new(g_direct_hash, g_direct_equal);
    if (_map_unfiltered_node_list == NULL) {
        free(_unfiltered_node_list_cache);
        return NULL;
    }
    for (_i = 0; _i < _unfiltered_node_list->size; _i++) {
        _node_id = _unfiltered_node_list->nodes[_i];
        g_hash_table_insert(_map_unfiltered_node_list, GINT_TO_POINTER(_node_id), GINT_TO_POINTER(_node_id));
    }
    _unfiltered_node_list_cache->map_unfiltered_node_list = _map_unfiltered_node_list;
    _unfiltered_node_list_cache->unfiltered_node_list = _unfiltered_node_list;
    return _unfiltered_node_list_cache;
}

void delete_unfiltered_node_list_cache(unfiltered_node_list_cache *_unfiltered_node_list_cache) {
    g_hash_table_destroy(_unfiltered_node_list_cache->map_unfiltered_node_list);
    free(_unfiltered_node_list_cache);
}

void set_unfiltered_node_list_cache(classifier *_classifier, unfiltered_node_list_cache *_unfiltered_node_list_cache) {
    _classifier->unfiltered_node_list_cache = _unfiltered_node_list_cache;
}

void set_map_term_node(classifier *_classifier, GHashTable *_map_term_node) {
    _classifier->map_term_node = _map_term_node;
}

void set_unfilterd_node(classifier *_classifier, unfiltered_node_list *_unfiltered_node_list) {
    _classifier->unfiltered_node_list = _unfiltered_node_list;
}

void set_map_term_weight_list(classifier *_classifier, GHashTable *_map_term_weight_list) {
    _classifier->map_term_weight_list = _map_term_weight_list;
}

void set_map_node_must_have(classifier *_classifier, GHashTable *_map_node_must_have) {
    _classifier->map_node_must_have = _map_node_must_have;
}

void delete_classifier(classifier *_classifier) {
    free(_classifier);
}

int create_classifier_maps(classifier_document *_classifier_document) {

    GHashTable *_map_document = NULL;
    GHashTable *_map_title = NULL;
    GHashTable *_map_summary = NULL;
    GHashTable *_map_node = NULL;
    GHashTable *_map_tax_node = NULL;
    _map_document = g_hash_table_new(g_direct_hash, g_direct_equal);
    if (_map_document == NULL) {
        return -1;
    }
    _map_title = g_hash_table_new(g_direct_hash, g_direct_equal);
    if (_map_title == NULL) {
        g_hash_table_destroy(_map_document);
        return -1;
    }
    _map_summary = g_hash_table_new(g_direct_hash, g_direct_equal);
    if (_map_summary == NULL) {
        g_hash_table_destroy(_map_document);
        g_hash_table_destroy(_map_title);
        return -1;
    }
    _map_node = g_hash_table_new(g_direct_hash, g_direct_equal);
    if (_map_node == NULL) {
        g_hash_table_destroy(_map_document);
        g_hash_table_destroy(_map_summary);
        g_hash_table_destroy(_map_title);
        return -1;
    }
    _map_tax_node = g_hash_table_new(g_direct_hash, g_direct_equal);
    if (_map_tax_node == NULL) {
        g_hash_table_destroy(_map_document);
        g_hash_table_destroy(_map_summary);
        g_hash_table_destroy(_map_title);
        g_hash_table_destroy(_map_node);
        return -1;
    }
    _classifier_document->map_document = _map_document;
    _classifier_document->map_title = _map_title;
    _classifier_document->map_summary = _map_summary;
    _classifier_document->map_node = _map_node;
    _classifier_document->map_tax_node = _map_tax_node;
    return 0;
}

int create_classifier_words_poll(classifier_document *_classifier_document, memory_page_buffer **_buffer) {
    word_poll *_word_poll_title = NULL;
    word_poll *_word_poll_summary = NULL;
    word_poll *_word_poll_document = NULL;
    _word_poll_title = create_word_poll(_buffer);
    if (_word_poll_title == NULL) {
        return -1;
    }
    _word_poll_document = create_word_poll(_buffer);
    if (_word_poll_document == NULL) {
        delete_word_poll(_word_poll_title);
        return -1;
    }
    _word_poll_summary = create_word_poll(_buffer);
    if (_word_poll_summary == NULL) {
        delete_word_poll(_word_poll_title);
        delete_word_poll(_word_poll_document);
        return -1;
    }
    _classifier_document->word_poll_summary = _word_poll_summary;
    _classifier_document->word_poll_document = _word_poll_document;
    _classifier_document->word_poll_title = _word_poll_title;
    return 0;
}

classifier_document *create_classifier_document(memory_page_buffer **_buffer) {
    classifier_document *_classifier_document = (classifier_document *) malloc(sizeof (classifier_document));
    node_score_buffer *_node_score_buffer = NULL;

    if (_classifier_document == NULL) {
        return NULL;
    }

    if (create_classifier_maps(_classifier_document) < 0) {
        free(_classifier_document);
        return NULL;
    }

    _node_score_buffer = create_node_score_buffer(_buffer);

    if (_node_score_buffer == NULL) {
        g_hash_table_destroy(_classifier_document->map_document);
        g_hash_table_destroy(_classifier_document->map_title);
        g_hash_table_destroy(_classifier_document->map_summary);
        g_hash_table_destroy(_classifier_document->map_node);
        g_hash_table_destroy(_classifier_document->map_tax_node);
        free(_classifier_document);

        return NULL;
    }
    if (create_classifier_words_poll(_classifier_document, _buffer) < 0) {
        g_hash_table_destroy(_classifier_document->map_document);
        g_hash_table_destroy(_classifier_document->map_title);
        g_hash_table_destroy(_classifier_document->map_summary);
        g_hash_table_destroy(_classifier_document->map_node);
        g_hash_table_destroy(_classifier_document->map_tax_node);
        free(_classifier_document);
        delete_node_score_buffer(_node_score_buffer);
        return NULL;
    }
    _classifier_document->node_score_buffer = _node_score_buffer;
    return _classifier_document;
}

void delete_classifier_document(classifier_document *_classifier_document) {
    g_hash_table_destroy(_classifier_document->map_document);
    g_hash_table_destroy(_classifier_document->map_title);
    g_hash_table_destroy(_classifier_document->map_summary);
    g_hash_table_destroy(_classifier_document->map_node);
    g_hash_table_destroy(_classifier_document->map_tax_node);
    delete_word_poll(_classifier_document->word_poll_title);
    delete_word_poll(_classifier_document->word_poll_document);
    delete_word_poll(_classifier_document->word_poll_summary);
    delete_node_score_buffer(_classifier_document->node_score_buffer);
    free(_classifier_document);
}

void clean_classifier_document(classifier_document *_classifier_document) {
    _classifier_document->node_score_buffer->current_size = 0;        
    clear_word_poll(_classifier_document->word_poll_document);
    clear_word_poll(_classifier_document->word_poll_summary);
    clear_word_poll(_classifier_document->word_poll_title);
    _classifier_document->title_size = 0;
    _classifier_document->document_size = 0;
    g_hash_table_remove_all(_classifier_document->map_document);
    g_hash_table_remove_all(_classifier_document->map_title);
    g_hash_table_remove_all(_classifier_document->map_summary);
    g_hash_table_remove_all(_classifier_document->map_node);
    g_hash_table_remove_all(_classifier_document->map_tax_node);
}
