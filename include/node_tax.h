#ifndef NODE_TAX_H
#define	NODE_TAX_H
#include "glib.h"
#include "memory_poll.h"
#include "memory_management.h"
#ifdef	__cplusplus
extern "C" {
#endif
    typedef struct _node_tax
    {
        uint32_t node_id;
        uint32_t tax_id;
    }node_tax;
    GHashTable *build_node_tax_mapping_from_node_tax_file(const char *node_tax_file, memory_page_buffer **page_buffer);
    void resolve_tax_on_node_score_buffer(node_score_buffer *buffer, GHashTable *node_tax_mapping);
#ifdef	__cplusplus
}
#endif

#endif

