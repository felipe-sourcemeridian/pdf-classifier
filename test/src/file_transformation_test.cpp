#include "file_transformation_test.h"
#include <iostream>
using namespace std;
const char *REMOVE_ACCENTS = "NFD;[:Nonspacing Mark:] Remove;NFC;Lower";
memory_page_buffer *page_buffer = NULL;
TEST(Steam, FileTransformation)
{
	const char *data = "ñoño fishing nada";
	uint32_t words_size_expected = 3;
	word_poll *words = create_word_poll(&page_buffer);
	file_transformation *trans = create_file_transformation_from_id(REMOVE_ACCENTS);
	char_buffer *buffer = create_char_buffer(&page_buffer);
	word_struct *word_nono = create_word("nono", 1);
	word_struct *word_fish = create_word("fish", 3);
	if(trans == NULL)
	{
		FAIL("fail to create file transformation \n");
		return;
	}
	g_hash_table_insert(trans->hwords, word_nono->word, GINT_TO_POINTER(word_nono->word_id));
	g_hash_table_insert(trans->hwords, word_fish->word, GINT_TO_POINTER(word_fish->word_id));
	memcpy(buffer->buffer, data, strlen(data));
	int resp = word_to_number(trans, buffer, words);
	CHECK(resp == 0);
	CHECK(words->current_size == words_size_expected);
	CHECK(words->words[0] == 1);
	CHECK(words->words[1] == 3);
	CHECK(words->words[2] == 0xffffffff);	
}

word_struct *create_word(const char *word_char, int id)
{
	word_struct *word = (word_struct *) malloc(sizeof(word_struct));
	word->size = strlen(word_char);
	word->word_id = id;
	memcpy((void *)word->word , word_char, word->size);
	return word;
}

file_transformation *create_file_transformation_from_id(const char *char_id)
{
	u16_char_buffer *remove_accents_id = create_id(char_id);
	GHashTable *words_mapping = g_hash_table_new(g_str_hash, g_str_equal);
	
	return create_file_transformation(&page_buffer, remove_accents_id, words_mapping);

}
