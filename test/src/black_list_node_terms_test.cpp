#include "TestHarness.h"
#include "black_list_node_terms.h"
#include <iostream>
#include <glib.h>
static const char *black_list_file_name = "data/black_list_node_terms.data";
static node_terms *create_node_terms(uint32_t node_id, uint32_t terms_size);
static bool assert_node_term_in_black_list(black_list_node_terms *black_list, node_terms *node_terms_expected);
static void add_term_to_document_mapping(GHashTable *document_mapping, uint32_t term_id);

TEST(BUILD_BLACK_LIST, BLACK_LIST_GROUP) {
    memory_page_buffer *page_buffer = NULL;
    node_terms *node_term_1 = create_node_terms(1588, 3);
    node_terms *node_term_2 = create_node_terms(2555, 2);
    black_list_node_terms *black_list = build_black_list_node_terms_from_file(&page_buffer, black_list_file_name);
    node_term_1->terms[0] = 2;
    node_term_1->terms[1] = 5;
    node_term_1->terms[2] = 6;
    node_term_2->terms[0] = 15;
    node_term_2->terms[1] = 16;
    CHECK(g_hash_table_size(black_list) == 2);
    CHECK(assert_node_term_in_black_list(black_list, node_term_1));
    CHECK(assert_node_term_in_black_list(black_list, node_term_2));
    free_memory_page_buffer(page_buffer);
    free(node_term_1);
    free(node_term_2);
}

TEST(IS_NODE_IN_BLACK_LIST, BLACK_LIST_GROUP) {
    GHashTable *document_mapping = g_hash_table_new(g_direct_hash, g_direct_equal);
    black_list_node_terms *black_list = g_hash_table_new(g_direct_hash, g_direct_equal);
    node_terms *node_term_1 = create_node_terms(1588, 3);
    node_terms *node_term_2 = create_node_terms(2555, 2);
    uint32_t wrong_node_id = 1215;
    add_term_to_document_mapping(document_mapping, 2);
    add_term_to_document_mapping(document_mapping, 8);
    add_term_to_document_mapping(document_mapping, 7);
    add_term_to_document_mapping(document_mapping, 10);
    add_term_to_document_mapping(document_mapping, 11);
    node_term_1->terms[0] = 2;
    node_term_1->terms[1] = 5;
    node_term_1->terms[2] = 6;
    node_term_2->terms[0] = 15;
    node_term_2->terms[1] = 16;
    g_hash_table_insert(black_list, GINT_TO_POINTER(node_term_1->node_id), node_term_1);
    g_hash_table_insert(black_list, GINT_TO_POINTER(node_term_2->node_id), node_term_2);
    CHECK(is_node_in_black_list(node_term_1->node_id, document_mapping, black_list));
    CHECK(is_node_in_black_list(node_term_2->node_id, document_mapping, black_list) == false);
    CHECK(is_node_in_black_list(wrong_node_id, document_mapping, black_list) == false);
}

static bool assert_node_term_in_black_list(black_list_node_terms *black_list, node_terms *node_terms_expected) {
    gpointer original_value;
    gpointer hash_value;
    node_terms *node_terms_return = NULL;
    bool is_equal = TRUE;
    if (g_hash_table_lookup_extended(black_list, GINT_TO_POINTER(node_terms_expected->node_id), &original_value, &hash_value) == false) {
        return false;
    }
    node_terms_return = (node_terms *) hash_value;
    is_equal = is_equal && node_terms_expected->node_id == node_terms_return->node_id;
    is_equal = is_equal && node_terms_expected->terms_size == node_terms_return->terms_size;
    if(!is_equal)
    {
        return is_equal;
    }
    for(uint32_t i=0; i<node_terms_expected->terms_size; i++)
    {
        is_equal = is_equal && node_terms_expected->terms[i] == node_terms_return->terms[i]; 
    }
    return is_equal;
    
}

static void add_term_to_document_mapping(GHashTable *document_mapping, uint32_t term_id)
{
    g_hash_table_insert(document_mapping, GINT_TO_POINTER(term_id), GINT_TO_POINTER(term_id));
}

static node_terms *create_node_terms(uint32_t node_id, uint32_t terms_size) {
    node_terms *current_node_term = (node_terms *) malloc(sizeof (node_terms) + sizeof (uint32_t) * terms_size);
    current_node_term->node_id = node_id;
    current_node_term->terms_size = terms_size;
    return current_node_term;
}