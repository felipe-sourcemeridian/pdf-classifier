#ifndef __TRANS__H
#define __TRANS__H
#include "stdlib.h"
#include <string.h>
#include <ctype.h>
#include <unicode/utrans.h>
#include <unicode/uenum.h>
#include <unicode/ustring.h>
#include <unicode/ustdio.h>
#include "memory_poll.h"
#include "string.h"
#define TESTANDSETSTATUS(var,status,value)	\
do						\
{						\
	if((var)==NULL)				\
	{					\
		(status)=(value);		\
		return NULL;			\
	}					\
}while(0)
#define TESTUFAILUREFREE(error_code,memory,status,value)	\
do								\
{								\
		      if(U_FAILURE((error_code)))		\
		      {						\
			      	free((memory));			\
				(status)=(value);		\
				return NULL;			\
		      }						\
}while(0)

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {TRANSFORMATION_ERROR,TRANSFORMATION_BAD_RULE,TRANSFORMATION_BAD_ID,TRANSFORMATION_OK} TRANSFORMATION_STATUS;
typedef enum {BUFFER_ERROR,BUFFER_MEMORY_ERROR,BUFFER_OK} BUFFER_STATUS;
typedef struct char_transformation
{
    u16_char_buffer *id;
    UTransliterator *transliterator;
} transformation;
void from_char_buffer(u16_char_buffer *ubuffer, char_buffer *buffer, BUFFER_STATUS *status);
transformation *create_transformation(u16_char_buffer *id, TRANSFORMATION_STATUS *status);
/*
 * Delete transformation object create previous with create_transformation please dont delete buffer inside transformation
 *@transformation *transformation
 * */
void delete_transformation(transformation *transformation);
/*
 * Transform give buffer by one transformation
 *@Param u16_char_buffer *buffer buffer to transform
 *@Param TRANSFORMATION_STATUS status this flags is the current status about the transformation the this buffer
 *@Param transformation *transformation
 * */
u16_char_buffer *transform(transformation *transformation, u16_char_buffer *buffer, TRANSFORMATION_STATUS *status);

#ifdef __cplusplus
}
#endif

#endif
