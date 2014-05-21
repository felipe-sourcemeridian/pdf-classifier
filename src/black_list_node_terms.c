#include "black_list_node_terms.h"
#include "syslog.h"

black_list_node_terms *build_black_list_node_terms_from_file(memory_page_buffer **page_buffer, const char *file_name) {
    black_list_node_terms *black_list_node = g_hash_table_new(g_direct_hash, g_direct_equal);
    char *black_list_page = (char *) map_file(file_name, page_buffer, FALSE);
    node_terms *current_node_terms = NULL;
    uint32_t black_list_size = 0;
    uint32_t size_current_node_terms = sizeof (node_terms);
    uint32_t size_term_id = sizeof (uint32_t);
    uint32_t current_index = 0;
    if (black_list_node == NULL) {
        syslog(LOG_ERR, "error to instance black list node terms\n");
        exit(1);
    }
    if (black_list_page == NULL) {
        syslog(LOG_ERR, "error to load black list node terms from file to memory\n");
        exit(1);
    }
    black_list_size = *((uint32_t *) black_list_page);
    black_list_page = black_list_page + size_term_id;
    for (current_index = 0; current_index < black_list_size; current_index++) {
        current_node_terms = (node_terms *) black_list_page;
        g_hash_table_insert(black_list_node, GINT_TO_POINTER(current_node_terms->node_id), current_node_terms);
        black_list_page = black_list_page + size_current_node_terms + (size_term_id * current_node_terms->terms_size);
    }
    return black_list_node;
}

int is_node_in_black_list(uint32_t node_id, GHashTable *document_mapping, black_list_node_terms *black_list) {
    gpointer original_value;
    gpointer hash_value;
    gpointer original_value_document;
    gpointer hash_value_document;
    uint32_t term_id = 0;
    node_terms *node_terms_black_list = NULL;
    uint32_t index;
    if (g_hash_table_lookup_extended(black_list, GINT_TO_POINTER(node_id), &original_value, &hash_value) == FALSE) {
        return FALSE;
    }
    node_terms_black_list = (node_terms *) hash_value;
    for (index = 0; index < node_terms_black_list->terms_size; index++) {
        term_id = node_terms_black_list->terms[index];
        if (g_hash_table_lookup_extended(document_mapping, GINT_TO_POINTER(term_id), &original_value_document, &hash_value_document) == TRUE) {
            return TRUE;
        }
    }
    return FALSE;
}