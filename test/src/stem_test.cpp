#include "TestHarness.h"
#include "string.h"
#include "porter.h"
#include <iostream>
using namespace std;
TEST(Steam, SteamGroup)
{
	char *buffer = (char *)	malloc(sizeof(char) * 20);
	const char *word_to_stemming = "iapetus";
	const char *word_expected = "iapetu";
	stemming *porter = (stemming *) malloc(sizeof(stemming));
	int length = strlen(word_to_stemming);
	memcpy(buffer, word_to_stemming, length);
	porter->k0 = 0;
        porter->j = 0;
        porter->k = length - 1;
	porter->buffer = buffer;	
	length = stem(porter);	
	porter->buffer[length + 1] = 0;
	CHECK(strcmp(word_expected, porter->buffer) == 0);
	free(buffer);
	free(porter);
}
