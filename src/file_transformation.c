#include "file_transformation.h"
file_transformation *create_file_transformation(memory_page_buffer **_buffer,u16_char_buffer *_id,GHashTable *_hwords)
{
	file_transformation *_file_transformation=(file_transformation *)malloc(sizeof(file_transformation));
	stemming *_stemming=NULL;
	transformation *_transformation=NULL;
	u16_char_buffer *_u16_buffer=NULL;
	TRANSFORMATION_STATUS _status;	
	if(_file_transformation==NULL)
	{
			return NULL;
	}
	_stemming=build_stemming(_buffer);
	if(_stemming==NULL)
	{	
		free(_file_transformation);	
		return NULL;
	}	
	_transformation=create_transformation(_id,&_status);
	if(_transformation==NULL)
	{
		delete_stemming(_stemming);
		free(_file_transformation);
		return NULL;
	}
	_u16_buffer=create_u16_char_buffer(_buffer);
	if(_u16_buffer==NULL)
	{	
		delete_stemming(_stemming);
		delete_transformation(_transformation);	
		free(_file_transformation);
		return NULL;
	}
	_file_transformation->hwords=_hwords;
	_file_transformation->stemming=_stemming;
	_file_transformation->transformation=_transformation;
	_file_transformation->buffer=_u16_buffer;
	return _file_transformation;
}
void delete_file_transformation(file_transformation *_file_transformation)
{
	delete_stemming(_file_transformation->stemming);
	delete_transformation(_file_transformation->transformation);
	delete_u16_char_buffer(_file_transformation->buffer);
	free(_file_transformation);
}
int transform_line(file_transformation *_file_transformation,char_buffer *_buffer)
{
	UErrorCode error;
	TRANSFORMATION_STATUS status;
	u16_char_buffer *_u16_buffer=_file_transformation->buffer;
	_u16_buffer->length=0;
	memset(&error,0,sizeof(UErrorCode));
	memset(&status,0,sizeof(TRANSFORMATION_STATUS));	
	u_strFromUTF8WithSub(_u16_buffer->data,_u16_buffer->capacity,&_u16_buffer->length,_buffer->buffer,_buffer->size,0xFFFD,NULL,&error);
	if(U_FAILURE(error))
	{
		printf("transliterator file\n");	
		return -1;
	}
	transform(_file_transformation->transformation,_u16_buffer,&status);
	if(status!=TRANSFORMATION_OK)
	{
		printf("transliterator file\n");	
		return -1;
	}

	u_strToUTF8WithSub(_buffer->buffer,_buffer->capacity,&_buffer->size,_u16_buffer->data,_u16_buffer->length,0xFFFD,NULL,&error);
	if(U_FAILURE(error))
	{	
		return -1;
	}
	return 0;
}
int word_to_number(file_transformation *_file_transformation,char_buffer *_buffer,word_poll *_word_poll)
{
	int _i=0;
	int _size=0;
	int _last=0;
	int _word_index=_word_poll->current_size;
	int _low_index=0;
	gpointer _original_key;
	gpointer _hash_value;
	stemming *_stemming=_file_transformation->stemming;
	if(transform_line(_file_transformation,_buffer)<0)
	{
		return -1;
	}
	if(!IS_LETTER(_buffer->buffer[0]))
	{
		_i = 1;	
	}
	_size=_buffer->size;	
	for(;_i<_size;_i++)
	{
		if(IS_NOT_WORD(_buffer->buffer[_last],_buffer->buffer[_i]))
		{
			_last=_i;
			continue;				
		}
		if(!IS_LETTER(_buffer->buffer[_i]))
		{	
			_last=_i;
			_stemming->buffer[_i - _low_index] = 0;
		        _stemming->k0 = 0;
            		_stemming->j = 0;
            		_stemming->k = strlen(_file_transformation->stemming->buffer) - 1;
            		_stemming->buffer[stem(_file_transformation->stemming) + 1]=0;
			if(_word_index==_word_poll->poll_size)
			{
				printf("*********** realloc memory file transformation *********\n");
				printf("*********** word size in realloc %d **********\n",_word_poll->current_size);
				realloc_word_poll(_word_poll,_word_poll->page->memory_size+PAGE_SIZE);
			}
			if(g_hash_table_lookup_extended(_file_transformation->hwords, _file_transformation->stemming->buffer, &_original_key, &_hash_value) == TRUE)
			{
/*				printf("************ word found %s *************\n",_stemming->buffer);*/
				_word_poll->words[_word_index]=GPOINTER_TO_INT(_hash_value);
			}
			else
			{
/*				printf("************ word not found %s *************\n",_stemming->buffer);*/
				_word_poll->words[_word_index]=(uint32_t)0xffffffff;
			}
			_word_poll->current_size=_word_poll->current_size+1;
			_word_index=_word_index+1;
			_low_index=_i+1;
			continue;
		}
		_last=_i;	
		_stemming->buffer[_i-_low_index]=_buffer->buffer[_i];
	}
	return 0;
}
