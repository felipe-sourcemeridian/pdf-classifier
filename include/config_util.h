#ifndef CONFIG_UTIL_H
#define CONFIG_UTIL_H
#include "glib.h"
#define STATIC_DATA_GROUP_NAME	"static_data"
#define STATE_AHO_FILE	"state_aho"
#define LIST_NODE_FILE	"list_node"
#define MAP_WORD_FILE	"map_word"
#define NODE_MUST_HAVE_FILE	"node_musthave"
#define NODE_SIG_VECTOR_FILE "node_sig_vector"
#define UNFILTERED_NODE_FILE "unfiltered_node"
#define NODE_TAX_FILE "node_tax"
#define SERVER_GROUP_NAME	"server"
#define SERVER_PORT	"server_port"
#define SERVER_MAX_REQUEST	"server_max_request"
#define SERVER_ACCEPT_TIMEOUT	"server_accept_timeout"
#define DAEMON_GROUP_NAME	"daemon"
#define DAEMON_WORKING_DIRECTORY "working_directory"
#define REQUEST_MANAGEMENT_GROUP_NAME	"request_management"
#define REQUEST_MANAGEMENT_MAX_REQUEST_POOL	"max_request_pool"
#define REQUEST_MANAGEMENT_WAIT_TIMEOUT	"request_wait_timeout"

#ifdef __cplusplus
extern "C" {
#endif

GKeyFile *load_config_file(const char *_config_file_name);

#ifdef __cplusplus
}
#endif

#endif
