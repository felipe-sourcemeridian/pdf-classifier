#include "TestHarness.h"
#include "node_tax.h"
#include <iostream>
using namespace std;
static node_score_buffer* create_node_score_buffer(uint32_t size);
static bool assert_node_tax(uint32_t node_id_expected, uint32_t tax_id_expected, node_tax *tax);
static void create_node_tax_on_mapping(GHashTable *mapping, uint32_t node_id, uint32_t tax_id);
static node_tax* get_node_tax_by_node_id(GHashTable *node_tax_mapping, uint32_t node_id);

TEST(BUILD_NODE_TAX_MAPPING_FROM_NODE_TAX_FILE, NODE_TAX_GROUP) {
    uint32_t node_id_expected_1 = 20120;
    uint32_t tax_id_expected_1 = 10001;
    uint32_t node_id_expected_2 = 20121;
    uint32_t tax_id_expected_2 = 10002;
    memory_page_buffer *page_buffer = NULL;
    GHashTable *node_tax_mapping = build_node_tax_mapping_from_node_tax_file("data/node_tax.data", &page_buffer);
    node_tax *tax_expected_1 = get_node_tax_by_node_id(node_tax_mapping, node_id_expected_1);
    node_tax *tax_expected_2 = get_node_tax_by_node_id(node_tax_mapping, node_id_expected_2);
    CHECK(tax_expected_1 != NULL);
    CHECK(tax_expected_2 != NULL);
    CHECK(g_hash_table_size(node_tax_mapping) == 2);
    CHECK(assert_node_tax(node_id_expected_1, tax_id_expected_1, tax_expected_1));
    CHECK(assert_node_tax(node_id_expected_2, tax_id_expected_2, tax_expected_2));
    free_memory_page_buffer(page_buffer);
}

TEST(RESOLVE_TAX_ON_NODE_SCORE_BUFFER, NODE_TAX_GROUP) {
    uint32_t node_id_expected_1 = 20120;
    uint32_t node_id_expected_2 = 20121;
    uint32_t tax_id_expected_1 = 10001;
    uint32_t tax_id_expected_2 = 10002;
    node_score_buffer *node_buffer = create_node_score_buffer(3);
    GHashTable *node_tax_mapping = g_hash_table_new(g_direct_hash, g_direct_equal);
    node_buffer->buffer[0].node_id = node_id_expected_1;
    node_buffer->buffer[1].node_id = node_id_expected_2;
    node_buffer->buffer[2].node_id = 0;
    create_node_tax_on_mapping(node_tax_mapping, node_id_expected_1, tax_id_expected_1);
    create_node_tax_on_mapping(node_tax_mapping, node_id_expected_2, tax_id_expected_2);
    resolve_tax_on_node_score_buffer(node_buffer, node_tax_mapping);
    CHECK(node_buffer->buffer[0].tax_id == tax_id_expected_1);
    CHECK(node_buffer->buffer[1].tax_id == tax_id_expected_2);
    CHECK(node_buffer->buffer[2].tax_id == 0);
    free(node_buffer);
    g_hash_table_destroy(node_tax_mapping);
}

static void create_node_tax_on_mapping(GHashTable *mapping, uint32_t node_id, uint32_t tax_id) {
    node_tax *tax = (node_tax *) malloc(sizeof (node_tax));
    tax->node_id = node_id;
    tax->tax_id = tax_id;
    g_hash_table_insert(mapping, GINT_TO_POINTER(node_id), tax);
}

static node_tax* get_node_tax_by_node_id(GHashTable *node_tax_mapping, uint32_t node_id) {
    gpointer original_value;
    gpointer hash_value;
    if (g_hash_table_lookup_extended(node_tax_mapping, GINT_TO_POINTER(node_id), &original_value, &hash_value) == false) {
        return NULL;
    }
    return (node_tax *) hash_value;
}

static bool assert_node_tax(uint32_t node_id_expected, uint32_t tax_id_expected, node_tax *tax) {
    bool is_equal = node_id_expected == tax->node_id;
    is_equal = is_equal && tax_id_expected == tax->tax_id;
    return is_equal;
}

static node_score_buffer* create_node_score_buffer(uint32_t size) {
    node_score_buffer *buffer = (node_score_buffer *) malloc(sizeof (node_score_buffer));
    buffer->current_size = size;
    buffer->node_score_poll_size = size;
    buffer->buffer = (node_score *) malloc(sizeof (node_score) * size);
    memset(buffer->buffer, 0, sizeof (node_score) * size);
    buffer->page = NULL;
    buffer->page_buffer = NULL;
    return buffer;
}
