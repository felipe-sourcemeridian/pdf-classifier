#include "trans.h"
void from_char_buffer(u16_char_buffer *ubuffer,char_buffer *buffer,BUFFER_STATUS *status)
{
    UErrorCode error;
    memset(&error,0,sizeof(UErrorCode));
    u_strToUTF8(buffer->buffer,buffer->capacity,&buffer->size,ubuffer->data,ubuffer->length,&error);
    if(U_FAILURE(error))
    {
        *status=BUFFER_ERROR;
        return;
    }    
    *status=BUFFER_OK;
}
void delete_transformation(transformation *transformation)
{
    utrans_close(transformation->transliterator);
    free(transformation);
}
transformation *create_transformation(u16_char_buffer *id,TRANSFORMATION_STATUS *status)
{
    transformation *_transformation=(transformation*)malloc(sizeof(transformation));
    UErrorCode error;
    UParseError parse_error;
    TESTANDSETSTATUS(_transformation,*status,TRANSFORMATION_ERROR);
    memset(&error,0,sizeof(UErrorCode));
    memset(&parse_error,0,sizeof(UParseError));
    _transformation->transliterator=utrans_openU(id->data, id->length, UTRANS_FORWARD, NULL, 0, &parse_error, &error);
    TESTUFAILUREFREE(error,_transformation,*status,TRANSFORMATION_ERROR);
    _transformation->id=id;
    *status=TRANSFORMATION_OK;
    return _transformation;
}
u16_char_buffer *transform(transformation *_transformation,u16_char_buffer *buffer,TRANSFORMATION_STATUS *status)
{
    UErrorCode error;
    TESTANDSETSTATUS(_transformation,*status,TRANSFORMATION_ERROR);
    memset(&error,0,sizeof(UErrorCode));
    utrans_transUChars(_transformation->transliterator,buffer->data,&buffer->length,buffer->capacity,0,&buffer->length, &error);
    if(U_FAILURE(error))
    {
	    *status=TRANSFORMATION_ERROR; 
	    return NULL;
    }
    *status=TRANSFORMATION_OK;
    return buffer;
}
