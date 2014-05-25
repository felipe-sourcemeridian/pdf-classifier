
#include "glib.h"

#include "state_aho_def.h"
#include "state_aho.h"
#include "builder_state_aho_fail_nodes.h"
static void add_state_node_to_node_to_processing(state_aho *current_node, GQueue *nodes_to_processing);
static void add_nodes_root_childs_and_set_fail_node(state_aho *root, GQueue *nodes_to_processing);
static state_aho *get_fail_node(state_aho* current_node, state_aho_cache *cache);
static state_aho* found_transition_in_node(uint32_t word_id, state_aho* node);

void build_state_aho_fail_node(state_aho_cache *phrases_tree) {
    GQueue *nodes_to_processing = g_queue_new();
    state_aho *root = phrases_tree->state_aho;
    state_aho *current_nodes = NULL;
    root->fail = root;
    add_nodes_root_childs_and_set_fail_node(root, nodes_to_processing);
    current_nodes = (state_aho *) g_queue_pop_head(nodes_to_processing);
    while (current_nodes != NULL) {
        add_state_node_to_node_to_processing(current_nodes, nodes_to_processing);
        current_nodes->fail = get_fail_node(current_nodes, phrases_tree);
        current_nodes = (state_aho *) g_queue_pop_head(nodes_to_processing);
    }
}

static state_aho *get_fail_node(state_aho* current_node, state_aho_cache *cache) {
    state_aho *parent = current_node->parent;
    state_aho *fail = NULL;
    state_aho *root = cache->state_aho;
    state_aho *node_trans = NULL;
    unsigned char has_more_fail_to_search = TRUE;
    while (has_more_fail_to_search) {
        fail = parent->fail;
        parent = fail;
        node_trans = found_transition_in_node(current_node->word_id, fail);
        has_more_fail_to_search = node_trans == NULL && (fail != root);
    }
    if (node_trans != NULL) {
        return node_trans;
    } else if (g_hash_table_lookup_extended(cache->cache, GINT_TO_POINTER(current_node->word_id), NULL, (void *) &node_trans) == TRUE) {
        return node_trans;
    }
    return root;
}

static state_aho* found_transition_in_node(uint32_t word_id, state_aho* node) {
    uint32_t index;
    uint32_t size = node->size;
    for (index = 0; index < size; index++) {
        if (node->childs[index]->word_id == word_id) {
            return node->childs[index];
        }
    }
    return NULL;
}

static void add_nodes_root_childs_and_set_fail_node(state_aho *root, GQueue *nodes_to_processing) {
    uint32_t index;
    uint32_t size = root->size;
    for (index = 0; index < size; index++) {
        root->childs[index]->fail = root;
        add_state_node_to_node_to_processing(root->childs[index], nodes_to_processing);
    }
}

static void add_state_node_to_node_to_processing(state_aho *current_node, GQueue *nodes_to_processing) {
    uint32_t index;
    uint32_t size = current_node->size;
    for (index = 0; index < size; index++) {
        g_queue_push_tail(nodes_to_processing, current_node->childs[index]);
    }
}