/* 
 * File:   black_list_node_terms.h
 * Author: root
 *
 * Created on May 21, 2014, 12:18 AM
 */

#ifndef BLACK_LIST_NODE_TERMS_H
#define	BLACK_LIST_NODE_TERMS_H
#include "glib.h"
#include "memory_management.h"
#include "memory_poll.h"
#ifdef	__cplusplus
extern "C" {
#endif
    typedef GHashTable black_list_node_terms;
    typedef struct
    {
        uint32_t node_id;
        uint32_t terms_size;
        uint32_t terms[0];
    }node_terms;
    black_list_node_terms *build_black_list_node_terms_from_file(memory_page_buffer **page_buffer ,const char *file_name);
    int is_node_in_black_list(uint32_t node_id, GHashTable *document_mapping,  black_list_node_terms *black_list);

#ifdef	__cplusplus
}
#endif

#endif	/* BLACK_LIST_NODE_TERMS_H */

