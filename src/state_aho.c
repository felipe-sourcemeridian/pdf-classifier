#include "state_aho.h"
static void add_new_transition_values(state_aho *transition, word_poll *words, uint32_t *offset, uint32_t *poll_size);
static void add_new_transition_value(uint32_t value, word_poll *words, uint32_t *offset, uint32_t *poll_size);
state_aho_cache *load_state_aho(const char *_file_name, memory_page_buffer **_buffer) {
    state_aho_cache *_state_aho_cache = NULL;
    GHashTable *_hcache = NULL;
    char *_base_address = NULL;
    _state_aho_cache = (state_aho_cache *) malloc(sizeof (state_aho_cache));
    if (_state_aho_cache == NULL) {
        return NULL;
    }
    _hcache = g_hash_table_new(g_direct_hash, g_direct_equal);
    if (_hcache == NULL) {
        free(_state_aho_cache);
        return NULL;
    }
    _state_aho_cache->cache = _hcache;
    _base_address = map_file(_file_name, _buffer, 0);
    if (_base_address == NULL) {
        delete_state_aho_cache(_state_aho_cache);
        return NULL;
    }
    fixed_pointers(_base_address, 0);
    _state_aho_cache->state_aho = (state_aho*) _base_address;
    build_cache(_state_aho_cache);
    return _state_aho_cache;
}

void delete_state_aho_cache(state_aho_cache *_state_aho_cache) {
    g_hash_table_destroy(_state_aho_cache->cache);
    free(_state_aho_cache);
}

void fixed_pointers(void *_base_pointer, uint64_t _offset) {
    uint64_t child_offset = 0;
    uint64_t parent_offset = 0;
    uint64_t fail_offset = 0;
    uint32_t size = 0;
    uint32_t i = 0;
    char *data = (char *) _base_pointer;
    state_aho *node = NULL;
    node = (state_aho *) (data + _offset);
    fail_offset = (uint64_t) (void *) node->fail;
    parent_offset = (uint64_t) (void *) node->parent;
    size = node->size;
    node->fail = (state_aho *) (data + fail_offset);
    node->parent = (state_aho *) (data + parent_offset);    
    for (i = 0; i < size; i++) {
        child_offset = (uint64_t) (void *) node->childs[i];
        node->childs[i] = (state_aho *) (data + child_offset);
        fixed_pointers(_base_pointer, child_offset);
    }
}

void build_cache(state_aho_cache *cache) {
    state_aho *root_node = cache->state_aho;
    int size = root_node->size;
    GHashTable *hcache = cache->cache;
    state_aho *node_child = NULL;
    int i = 0;
    for (i = 0; i < size; i++) {
        node_child = root_node->childs[i];        
        g_hash_table_insert(hcache, GINT_TO_POINTER(node_child->word_id), node_child);
    }
}

void search_words(state_aho_cache *cache, word_poll *words) {
    uint32_t original_size = words->current_size;
    uint32_t i = 0;
    uint32_t offset = original_size;
    uint32_t poll_size = words->poll_size;
    state_aho *current_node = cache->state_aho;
    state_aho *transtition = NULL;
    while (i < original_size) {
        transtition = get_transitions(words->words[i], current_node, cache);
        if (transtition == NULL) {
            transtition = get_node_transition_on_node_fail(cache, current_node->fail, words->words[i]);
        }
        if (transtition->has_result == 1 || transtition->fail->has_result == 1) {
            add_new_transition_values(transtition, words, &offset,&poll_size);
        }
        current_node = transtition;
        i++;
    }
}

static void add_new_transition_values(state_aho *transition, word_poll *words, uint32_t *offset, uint32_t *poll_size) {
    if(transition->has_result == 1)
    {
        add_new_transition_value(transition->value, words, offset, poll_size);
    }
    if(transition->fail->has_result == 1)
    {
        add_new_transition_value(transition->fail->value, words, offset, poll_size);
    }
}

static void add_new_transition_value(uint32_t value, word_poll *words, uint32_t *offset, uint32_t *poll_size) {
    if (*offset >= *poll_size) {
        realloc_word_poll(words, words->page->memory_size + PAGE_SIZE);
        *poll_size = words->poll_size;
    }
    words->words[*offset] = value;
    *offset = *offset + 1;
    words->current_size = words->current_size + 1;
}

state_aho *get_node_transition_on_node_fail(state_aho_cache *_state_aho_cache, state_aho *node_fail, uint32_t word_id) {
    state_aho *node_transition = NULL;
    state_aho *root_node = _state_aho_cache->state_aho;
    while (root_node != node_fail) {
        node_transition = get_transitions(word_id, node_fail, _state_aho_cache);
        if (node_transition == NULL) {
            node_fail = node_fail->fail;
            continue;
        }
        return node_transition;
    }
    node_transition = get_transitions(word_id, root_node, _state_aho_cache);
    if (node_transition != NULL) {
        return node_transition;
    }
    return root_node;

}

state_aho *get_transitions(uint32_t _word_id, state_aho *_current_node, state_aho_cache *_state_aho_cache) {
    state_aho *transition = NULL;
    GHashTable *hcache = _state_aho_cache->cache;
    uint32_t size = _current_node->size;
    uint32_t i = 0;
    if (_current_node == _state_aho_cache->state_aho) {
        if (g_hash_table_lookup_extended(hcache, GINT_TO_POINTER(_word_id), NULL, (void *) &transition) == TRUE) {
            return transition;
        }
        return NULL;
    }
    for (i = 0; i < size; i++) {
        if (_current_node->childs[i]->word_id == _word_id) {
            transition = _current_node->childs[i];
            break;
        }
    }
    return transition;
}
