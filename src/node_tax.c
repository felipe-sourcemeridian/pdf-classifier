#include "node_tax.h"
#include "syslog.h"
#include "porter.h"
GHashTable *build_node_tax_mapping_from_node_tax_file(const char *node_tax_file, memory_page_buffer **page_buffer)
{
    GHashTable *node_tax_mapping = g_hash_table_new(g_direct_hash, g_direct_equal);    
    void *node_tax_page = map_file(node_tax_file, page_buffer, FALSE);    
    uint32_t node_tax_size = 0;    
    node_tax *node_tax_buffer = NULL;
    int i = 0;
    if(node_tax_page == NULL)
    {
        syslog(LOG_ERR, "node tax file %s not found or is not super user %s\n", node_tax_file);
        exit(1);
    }
    if(node_tax_mapping == NULL)
    {
        syslog(LOG_ERR, "not memory viable\n");
        exit(1);
    }
    node_tax_size = *((uint32_t *)node_tax_page);
    node_tax_page = ((uint32_t *)node_tax_page) + 1;
    node_tax_buffer = (node_tax *) node_tax_page;
    for(i = 0; i< node_tax_size; i++)
    {
        g_hash_table_insert(node_tax_mapping, GINT_TO_POINTER(node_tax_buffer->node_id), node_tax_buffer);
        node_tax_buffer = node_tax_buffer + 1;
    }
    return node_tax_mapping;
}

void resolve_tax_on_node_score_buffer(node_score_buffer *buffer, GHashTable *node_tax_mapping) 
{
    uint32_t node_score_size = buffer->current_size;
    uint32_t node_id;    
    node_tax *tax;
    gpointer original_hash_key;
    gpointer hash_value;
    int i = 0;
    for(i = 0; i < node_score_size; i++)
    {
        node_id = buffer->buffer[i].node_id;
        if(g_hash_table_lookup_extended(node_tax_mapping, GINT_TO_POINTER(node_id), &original_hash_key, &hash_value) == FALSE)
        {
            continue;
        }
        tax = (node_tax *)hash_value;
        buffer->buffer[i].tax_id = tax->tax_id;
    }
}
