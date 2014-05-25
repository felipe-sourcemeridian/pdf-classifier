#include "utility.h"
#include "syslog.h"

u16_char_buffer *create_id(const char *_id) {
    int32_t _length = 0;
    int32_t _capacity = 0;
    UErrorCode _pErrorCode;
    u16_char_buffer *_buffer = (u16_char_buffer *) malloc(sizeof (u16_char_buffer));
    if (_buffer == NULL) {
        return NULL;
    }
    _capacity = strlen(_id);
    _buffer->data = (UChar *) malloc(sizeof (UChar) * _capacity);
    if (_buffer->data == NULL) {
        free(_buffer);
        return NULL;
    }
    memset(&_pErrorCode, 0, sizeof (UErrorCode));
    u_strFromUTF8(_buffer->data, _capacity, &_length, _id, _capacity, &_pErrorCode);
    if (U_FAILURE(_pErrorCode)) {
        return NULL;
    }
    _buffer->length = _capacity;
    return _buffer;
}

void delete_id(u16_char_buffer *_buffer) {
    free(_buffer->data);
    free(_buffer);
}

classifier *build_classifier(GKeyFile *_config_file, memory_page_buffer **_buffer) {
    GHashTable *_map_term_node = NULL;
    GHashTable *_map_term_weigth_list = NULL;
    GHashTable *_map_node_must_have = NULL;
    char *_file_name_state_aho = NULL;
    char *_file_name_list_node = NULL;
    char *_file_name_node_musthave = NULL;
    char *_file_name_node_sig_vector = NULL;
    char *_file_name_unfiltered_node = NULL;
    char *node_tax_file_name = NULL;
    char *black_list_node_terms_file_name = NULL;
    state_aho_cache *_state_aho_cache = NULL;
    classifier *_classifier = NULL;
    unfiltered_node_list *_unfiltered_node_list = NULL;
    unfiltered_node_list_cache *_unfiltered_node_list_cache = NULL;
    GHashTable *node_tax_mapping = NULL;
    black_list_node_terms *black_list = NULL;
    _file_name_state_aho = g_key_file_get_string(_config_file, STATIC_DATA_GROUP_NAME, STATE_AHO_FILE, NULL);
    _file_name_list_node = g_key_file_get_string(_config_file, STATIC_DATA_GROUP_NAME, LIST_NODE_FILE, NULL);
    _file_name_node_musthave = g_key_file_get_string(_config_file, STATIC_DATA_GROUP_NAME, NODE_MUST_HAVE_FILE, NULL);
    _file_name_node_sig_vector = g_key_file_get_string(_config_file, STATIC_DATA_GROUP_NAME, NODE_SIG_VECTOR_FILE, NULL);
    _file_name_unfiltered_node = g_key_file_get_string(_config_file, STATIC_DATA_GROUP_NAME, UNFILTERED_NODE_FILE, NULL);
    node_tax_file_name = g_key_file_get_string(_config_file, STATIC_DATA_GROUP_NAME, NODE_TAX_FILE, NULL);
    black_list_node_terms_file_name = g_key_file_get_string(_config_file, STATIC_DATA_GROUP_NAME, BLACK_LIST_NODE_TERMS_FILE, NULL);
    if(_file_name_list_node == NULL)
    {
        syslog(LOG_ERR, "node list is not set on configuration's file\n");
        return NULL;
    }
    if (_file_name_state_aho == NULL ) {
        syslog(LOG_ERR, "phrases tree is not set on configuration's file\n");
        return NULL;
    }
    if(node_tax_file_name == NULL)
    {
        syslog(LOG_ERR, "node tax file is not set on configuration's file\n");
        return NULL;
    }
    if(black_list_node_terms_file_name == NULL)
    {
        syslog(LOG_ERR, "black list node terms file is not set on configuration's file\n");
        return NULL;
    }
    if(_file_name_node_musthave == NULL)
    {
        syslog(LOG_ERR, "node must have file is not set on configuration's file\n");
        return NULL;
    }
    if(_file_name_node_sig_vector == NULL)
    {
        syslog(LOG_ERR, "node sig vector file is not set on configuration's file\n");
        return NULL;
    }
    if (  _file_name_unfiltered_node == NULL) 
    {
        syslog(LOG_ERR, "unfiltered node file is not set on configuration's file\n");
        return NULL;
    }
    _state_aho_cache = load_state_aho(_file_name_state_aho, _buffer);
    _map_term_node = load_list_node(_file_name_list_node, _buffer);
    _map_node_must_have = load_node_musthave_term(_file_name_node_musthave, _buffer);
    _map_term_weigth_list = load_node_sig_term(_file_name_node_sig_vector, _buffer);
    _unfiltered_node_list = load_unfiltered_nodes(_file_name_unfiltered_node, _buffer);
    node_tax_mapping = build_node_tax_mapping_from_node_tax_file(node_tax_file_name, _buffer);
    black_list = build_black_list_node_terms_from_file(_buffer, black_list_node_terms_file_name);
    if (_state_aho_cache == NULL || _map_term_node == NULL) {
        return NULL;
    }
    if (_unfiltered_node_list == NULL || _map_node_must_have == NULL || _map_term_weigth_list == NULL) {
        return NULL;
    }
    _unfiltered_node_list_cache = build_unfiltered_node_list_cache(_unfiltered_node_list);
    if (_unfiltered_node_list_cache == NULL) {
        return NULL;
    }
    _classifier = create_classifier(_state_aho_cache);
    if (_classifier == NULL) {
        return NULL;
    }
    g_free(_file_name_state_aho);
    g_free(_file_name_list_node);
    g_free(_file_name_node_musthave);
    g_free(_file_name_node_sig_vector);
    g_free(_file_name_unfiltered_node);
    g_free(node_tax_file_name);
    g_free(black_list_node_terms_file_name);
    set_unfilterd_node(_classifier, _unfiltered_node_list);
    set_map_term_node(_classifier, _map_term_node);
    set_map_node_must_have(_classifier, _map_node_must_have);
    set_map_term_weight_list(_classifier, _map_term_weigth_list);
    set_unfiltered_node_list_cache(_classifier, _unfiltered_node_list_cache);
    _classifier->node_tax_mapping = node_tax_mapping;
    _classifier->black_list = black_list;
    return _classifier;
}
