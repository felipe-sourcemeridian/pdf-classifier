#include "TestHarness.h"
#include "string.h"
#include "porter.h"
#include <iostream>
#include "stdlib.h"
using namespace std;
static bool test_stem_term(const char *term, const char* term_expected, struct stemmer *porter);

TEST(Steam, SteamGroup) {
    char *buffer = (char *) malloc(sizeof (char) * 1024);
    struct stemmer *porter = (struct stemmer *) malloc(sizeof (struct stemmer));
    porter->b = buffer;
    CHECK(test_stem_term("caresses", "caress", porter));
    CHECK(test_stem_term("ponies", "poni", porter));
    CHECK(test_stem_term("ties", "ti", porter));
    CHECK(test_stem_term("caress", "caress", porter));
    CHECK(test_stem_term("cats", "cat", porter));
    CHECK(test_stem_term("feed", "feed", porter));
    CHECK(test_stem_term("agreed", "agre", porter));
    CHECK(test_stem_term("plastered", "plaster", porter));
    CHECK(test_stem_term("bled", "bled", porter));
    CHECK(test_stem_term("motoring", "motor", porter));
    CHECK(test_stem_term("sing", "sing", porter));
    CHECK(test_stem_term("conflated", "conflat", porter));
    CHECK(test_stem_term("troubled", "troubl", porter));
    CHECK(test_stem_term("sized", "size", porter));
    CHECK(test_stem_term("hopping", "hop", porter));
    CHECK(test_stem_term("tanned", "tan", porter));
    CHECK(test_stem_term("falling", "fall", porter));
    CHECK(test_stem_term("hissing", "hiss", porter));
    CHECK(test_stem_term("fizzed", "fizz", porter));
    CHECK(test_stem_term("failing", "fail", porter));
    CHECK(test_stem_term("filing", "file", porter));
    CHECK(test_stem_term("happy", "happi", porter));
    CHECK(test_stem_term("sky", "sky", porter));
    CHECK(test_stem_term("relational", "relat", porter));
    CHECK(test_stem_term("conditional", "condit", porter));
    CHECK(test_stem_term("rational", "ration", porter));
    CHECK(test_stem_term("valenci", "valenc", porter));
    CHECK(test_stem_term("hesitanci", "hesit", porter));
    CHECK(test_stem_term("digitizer", "digit", porter));
    CHECK(test_stem_term("conformabli", "conform", porter));
    CHECK(test_stem_term("radicalli", "radic", porter));
    CHECK(test_stem_term("differentli", "differ", porter));
    CHECK(test_stem_term("vileli", "vile", porter));
    CHECK(test_stem_term("analogousli", "analog", porter));
    CHECK(test_stem_term("vietnamization", "vietnam", porter));
    CHECK(test_stem_term("predication", "predic", porter));
    CHECK(test_stem_term("operator", "oper", porter));    
    free(buffer);
    free(porter);
}

static bool test_stem_term(const char *term, const char* term_expected, struct stemmer *porter) {
    int length = strlen(term);
    memcpy(porter->b, term, length);
    porter->j = 0;
    porter->k = length - 1;
    length = stem(porter);
    porter->b[length + 1] = 0;
    cout << term << "->" << term_expected << ": " << porter->b << endl;
    return strcmp(term_expected, porter->b) == 0;
    
}