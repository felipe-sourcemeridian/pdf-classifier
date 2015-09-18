#include "file_transformation_test.h"
#include <iostream>
using namespace std;
const char *REMOVE_ACCENTS = "NFD;[:Nonspacing Mark:] Remove;NFC;Lower";
memory_page_buffer *page_buffer = NULL;
TEST(Steam, FileTransformation)
{
	const char *data = "ñoño fishing nada G.H.O.S.T G.H.O.S.T. G.H.O.S.t. CARLOS-VIERA G.H.O.S.T.2";
	uint32_t words_size_expected = 10;
	word_poll *words = create_word_poll(&page_buffer);
	file_transformation *trans = create_file_transformation_from_id(REMOVE_ACCENTS);
	char_buffer *buffer = create_char_buffer(&page_buffer);
	word_struct *word_nono = create_word("nono", 1);
	word_struct *word_fish = create_word("fishing", 3);        
        word_struct *word_ghost = create_word("GHOST", 4);                
        word_struct *word_ghost3 = create_word("ghost", 6); 
        word_struct *word_carlos = create_word("CARLOS", 7); 
        word_struct *word_viera = create_word("VIERA", 8); 
        word_struct *word_ghost_number = create_word("GHOST2", 9); 
	if(trans == NULL)
	{
		FAIL("fail to create file transformation \n");
		return;
	}
	g_hash_table_insert(trans->hwords, word_nono->word, GINT_TO_POINTER(word_nono->word_id));
	g_hash_table_insert(trans->hwords, word_fish->word, GINT_TO_POINTER(word_fish->word_id));
        g_hash_table_insert(trans->hwords, word_ghost->word, GINT_TO_POINTER(word_ghost->word_id));        
        g_hash_table_insert(trans->hwords, word_ghost3->word, GINT_TO_POINTER(word_ghost3->word_id));        
        g_hash_table_insert(trans->hwords, word_carlos->word, GINT_TO_POINTER(word_carlos->word_id));        
        g_hash_table_insert(trans->hwords, word_viera->word, GINT_TO_POINTER(word_viera->word_id));        
        g_hash_table_insert(trans->hwords, word_ghost_number->word, GINT_TO_POINTER(word_ghost_number->word_id));                
	memcpy(buffer->buffer, data, strlen(data));
        buffer->size = strlen(data);
	int resp = word_to_number(trans, buffer, words);
	CHECK(resp == 0);
	CHECK(words->current_size == words_size_expected);
	CHECK(words->words[0] == 1);
	CHECK(words->words[1] == 3);
	CHECK(words->words[2] == WORD_ID_NOT_FOUND);	
        CHECK(words->words[3] == 4);	
        CHECK(words->words[4] == 4);	
        CHECK(words->words[5] == 6);	
        CHECK(words->words[6] == 7);	
        CHECK(words->words[7] == 8);	
        CHECK(words->words[8] == 9);	
        CHECK(words->words[9] == WORD_ID_NOT_FOUND);	
}

word_struct *create_word(const char *word_char, int id)
{
	word_struct *word = NULL;                
        int32_t word_size = strlen(word_char)+1;
        word = (word_struct *) malloc(sizeof(word_struct) + word_size);
	word->size = word_size;
	word->word_id = id;                
	memcpy((void *)&word->word, word_char, word->size);
	return word;
}

file_transformation *create_file_transformation_from_id(const char *char_id)
{
	u16_char_buffer *remove_accents_id = create_id(char_id);
	GHashTable *words_mapping = g_hash_table_new(g_str_hash, g_str_equal);
	
	return create_file_transformation(&page_buffer, remove_accents_id, words_mapping);

}

u16_char_buffer *create_id(const char *_id)
{
	int32_t _length=0;
	int32_t _capacity=0;
	UErrorCode _pErrorCode;
	u16_char_buffer *_buffer=(u16_char_buffer *)malloc(sizeof(u16_char_buffer));
	if(_buffer==NULL)
	{
		return NULL;
	}
	_capacity=strlen(_id);
	_buffer->data=(UChar *)malloc(sizeof(UChar)*_capacity);
	if(_buffer->data==NULL)
	{
		free(_buffer);
		return NULL;
	}
	memset(&_pErrorCode,0,sizeof(UErrorCode));	
	u_strFromUTF8(_buffer->data,_capacity,&_length,_id,_capacity,&_pErrorCode);
	if(U_FAILURE(_pErrorCode))
	{	
		return NULL;
	}
	_buffer->length=_capacity;
	return _buffer;
}
