#ifndef STATE_AHO_DEF
#define STATE_AHO_DEF
#include "inttypes.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _state_aho
{
	 struct _state_aho *parent;
	 uint32_t word_id;
	 uint32_t value;
	 struct _state_aho *fail;
	 uint32_t size;
	 unsigned char has_result;
	 unsigned char is_null_node;
	 char padding[2];
	 struct _state_aho *childs[0];
};
typedef struct _state_aho state_aho;

#ifdef __cplusplus
}
#endif

#endif
