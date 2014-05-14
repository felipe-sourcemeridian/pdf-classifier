#include "TestHarness.h"
#include "string.h"
#include "score_document.h"
#include <iostream>
#include "stdlib.h"
using namespace std;
static node_score_buffer* create_node_score_buffer(uint32_t size);
static classifier_document *create_classifier_document();
static void free_node_score_buffer(node_score_buffer *buffer);

TEST(DO_COMBINE_SCORE_ROC_0_CASE_1, SCORE_GROUP)
{
    classifier_document *document = create_classifier_document();
    node_score_buffer *node_buffer = create_node_score_buffer(1);
    document->node_score_buffer = node_buffer;
    node_buffer->buffer[0].frequency = 1.6;
    node_buffer->buffer[0].coverage = 0.2;     
    do_combine_score(document);
    CHECK(node_buffer->buffer[0].roc == 0);
    free_node_score_buffer(node_buffer);
    free(document);
}

TEST(DO_COMBINE_SCORE_ROC_0_CASE_2, SCORE_GROUP)
{
    classifier_document *document = create_classifier_document();
    node_score_buffer *node_buffer = create_node_score_buffer(1);
    document->node_score_buffer = node_buffer;
    node_buffer->buffer[0].frequency = 1.4;
    node_buffer->buffer[0].coverage = 0.2;     
    node_buffer->buffer[0].flags = TITLE_MATCH;
    do_combine_score(document);
    CHECK(node_buffer->buffer[0].roc == 0);
    free_node_score_buffer(node_buffer);
    free(document);
}

TEST(DO_COMBINE_SCORE_ROC_0_CASE_4, SCORE_GROUP)
{
    classifier_document *document = create_classifier_document();
    node_score_buffer *node_buffer = create_node_score_buffer(1);
    document->node_score_buffer = node_buffer;
    node_buffer->buffer[0].frequency = 4.88;
    node_buffer->buffer[0].coverage = 0.125;
    do_combine_score(document);
    CHECK(node_buffer->buffer[0].roc == 0);
    free_node_score_buffer(node_buffer);
    free(document);
}


TEST(DO_COMBINE_SCORE_ROC_0_CASE_6, SCORE_GROUP)
{
    classifier_document *document = create_classifier_document();
    node_score_buffer *node_buffer = create_node_score_buffer(1);
    document->node_score_buffer = node_buffer;
    node_buffer->buffer[0].frequency = 6.1;
    node_buffer->buffer[0].coverage = 0.11;
    do_combine_score(document);
    CHECK(node_buffer->buffer[0].roc == 0);
    free_node_score_buffer(node_buffer);
    free(document);
}

TEST(DO_COMBINE_SCORE_ROC_0_CASE_7, SCORE_GROUP)
{
    classifier_document *document = create_classifier_document();
    node_score_buffer *node_buffer = create_node_score_buffer(1);
    document->node_score_buffer = node_buffer;
    node_buffer->buffer[0].frequency = 9.7;
    node_buffer->buffer[0].coverage = 0.11;
    do_combine_score(document);
    CHECK(node_buffer->buffer[0].roc == 0);
    free_node_score_buffer(node_buffer);
    free(document);
}



TEST(DO_COMBINE_SCORE_ROC_50, SCORE_GROUP)
{
    classifier_document *document = create_classifier_document();
    node_score_buffer *node_buffer = create_node_score_buffer(1);
    document->node_score_buffer = node_buffer;
    node_buffer->buffer[0].frequency = 1.0;
    node_buffer->buffer[0].coverage = 0.34;     
    do_combine_score(document);
    CHECK(node_buffer->buffer[0].roc == 50);
    free_node_score_buffer(node_buffer);
    free(document);
}

TEST(DO_COMBINE_SCORE_ROC_50_WITH_MUST_SUMMARY, SCORE_GROUP)
{
    classifier_document *document = create_classifier_document();
    node_score_buffer *node_buffer = create_node_score_buffer(1);
    document->node_score_buffer = node_buffer;
    node_buffer->buffer[0].frequency = 1.0;
    node_buffer->buffer[0].coverage = 0.34; 
    node_buffer->buffer[0].flags = NODE_TOP;
    do_combine_score(document);
    CHECK(node_buffer->buffer[0].roc == 50);
    free_node_score_buffer(node_buffer);
    free(document);
}

TEST(DO_COMBINE_SCORE_ROC_55, SCORE_GROUP)
{
    classifier_document *document = create_classifier_document();
    node_score_buffer *node_buffer = create_node_score_buffer(1);
    document->node_score_buffer = node_buffer;
    node_buffer->buffer[0].frequency = 1.0;
    node_buffer->buffer[0].coverage = 0.34; 
    node_buffer->buffer[0].flags = TITLE_MATCH;
    do_combine_score(document);
    CHECK(node_buffer->buffer[0].roc == 55);
    free_node_score_buffer(node_buffer);
    free(document);
}

TEST(DO_COMBINE_SCORE_ROC_55_WITH_TILE_AND_SUMMARY, SCORE_GROUP)
{
    classifier_document *document = create_classifier_document();
    node_score_buffer *node_buffer = create_node_score_buffer(1);
    document->node_score_buffer = node_buffer;
    node_buffer->buffer[0].frequency = 1.0;
    node_buffer->buffer[0].coverage = 0.34; 
    node_buffer->buffer[0].flags = TITLE_WITH_TOP;
    do_combine_score(document);
    CHECK(node_buffer->buffer[0].roc == 55);
    free_node_score_buffer(node_buffer);
    free(document);
}

TEST(DO_COMBINE_SCORE_ROC_60, SCORE_GROUP)
{
    classifier_document *document = create_classifier_document();
    node_score_buffer *node_buffer = create_node_score_buffer(1);
    document->node_score_buffer = node_buffer;
    node_buffer->buffer[0].frequency = 4.69f;
    node_buffer->buffer[0].coverage = 0.130f;     
    do_combine_score(document);
    CHECK(node_buffer->buffer[0].roc == 60);
    free_node_score_buffer(node_buffer);
    free(document);
}

TEST(DO_COMBINE_SCORE_ROC_60_WITH_SUMMARY, SCORE_GROUP)
{
    classifier_document *document = create_classifier_document();
    node_score_buffer *node_buffer = create_node_score_buffer(1);
    document->node_score_buffer = node_buffer;
    node_buffer->buffer[0].frequency = 4.69f;
    node_buffer->buffer[0].coverage = 0.130f; 
    node_buffer->buffer[0].flags = NODE_TOP;
    do_combine_score(document);
    CHECK(node_buffer->buffer[0].roc == 60);
    free_node_score_buffer(node_buffer);
    free(document);
}


TEST(DO_COMBINE_SCORE_ROC_65, SCORE_GROUP)
{
    classifier_document *document = create_classifier_document();
    node_score_buffer *node_buffer = create_node_score_buffer(1);
    document->node_score_buffer = node_buffer;
    node_buffer->buffer[0].frequency = 4.69f;
    node_buffer->buffer[0].coverage = 0.130f; 
    node_buffer->buffer[0].flags = TITLE_MATCH;
    do_combine_score(document);
    CHECK(node_buffer->buffer[0].roc == 65);
    free_node_score_buffer(node_buffer);
    free(document);
}

TEST(DO_COMBINE_SCORE_ROC_65_WITH_TITLE_AND_SUMMARY, SCORE_GROUP)
{
    classifier_document *document = create_classifier_document();
    node_score_buffer *node_buffer = create_node_score_buffer(1);
    document->node_score_buffer = node_buffer;
    node_buffer->buffer[0].frequency = 4.69f;
    node_buffer->buffer[0].coverage = 0.130f; 
    node_buffer->buffer[0].flags = TITLE_WITH_TOP;
    do_combine_score(document);
    CHECK(node_buffer->buffer[0].roc == 65);
    free_node_score_buffer(node_buffer);
    free(document);
}


TEST(DO_COMBINE_SCORE_ROC_75, SCORE_GROUP)
{
    classifier_document *document = create_classifier_document();
    node_score_buffer *node_buffer = create_node_score_buffer(1);
    document->node_score_buffer = node_buffer;
    node_buffer->buffer[0].frequency = 1.0;
    node_buffer->buffer[0].coverage = 1.0;     
    do_combine_score(document);
    CHECK(node_buffer->buffer[0].roc == 75);
    free_node_score_buffer(node_buffer);
    free(document);
}

TEST(DO_COMBINE_SCORE_ROC_75_WITH_SUMMARY, SCORE_GROUP)
{
    classifier_document *document = create_classifier_document();
    node_score_buffer *node_buffer = create_node_score_buffer(1);
    document->node_score_buffer = node_buffer;
    node_buffer->buffer[0].frequency = 1.0;
    node_buffer->buffer[0].coverage = 1.0; 
    node_buffer->buffer[0].flags = NODE_TOP;
    do_combine_score(document);
    CHECK(node_buffer->buffer[0].roc == 75);
    free_node_score_buffer(node_buffer);
    free(document);
}


TEST(DO_COMBINE_SCORE_ROC_80, SCORE_GROUP)
{
    classifier_document *document = create_classifier_document();
    node_score_buffer *node_buffer = create_node_score_buffer(1);
    document->node_score_buffer = node_buffer;
    node_buffer->buffer[0].frequency = 1.0;
    node_buffer->buffer[0].coverage = 1.0; 
    node_buffer->buffer[0].flags = TITLE_MATCH;
    do_combine_score(document);
    CHECK(node_buffer->buffer[0].roc == 80);
    free_node_score_buffer(node_buffer);
    free(document);
}

TEST(DO_COMBINE_SCORE_ROC_80_WITH_TITLE_AND_SUMMARY, SCORE_GROUP)
{
    classifier_document *document = create_classifier_document();
    node_score_buffer *node_buffer = create_node_score_buffer(1);
    document->node_score_buffer = node_buffer;
    node_buffer->buffer[0].frequency = 1.0;
    node_buffer->buffer[0].coverage = 1.0; 
    node_buffer->buffer[0].flags = TITLE_WITH_TOP;
    do_combine_score(document);
    CHECK(node_buffer->buffer[0].roc == 80);
    free_node_score_buffer(node_buffer);
    free(document);
}

TEST(DO_COMBINE_SCORE_ROC_100, SCORE_GROUP)
{
    classifier_document *document = create_classifier_document();
    node_score_buffer *node_buffer = create_node_score_buffer(1);
    document->node_score_buffer = node_buffer;
    node_buffer->buffer[0].frequency = 2.0f;
    node_buffer->buffer[0].coverage = 1.0f;     
    do_combine_score(document);
    CHECK(node_buffer->buffer[0].roc == 100);
    free_node_score_buffer(node_buffer);
    free(document);
}

TEST(DO_COMBINE_SCORE_ROC_100_WITH_TITLE_MUST_HAVE, SCORE_GROUP)
{
    classifier_document *document = create_classifier_document();
    node_score_buffer *node_buffer = create_node_score_buffer(1);
    document->node_score_buffer = node_buffer;
    node_buffer->buffer[0].frequency = 2.0;
    node_buffer->buffer[0].coverage = 1.0; 
    node_buffer->buffer[0].flags = TITLE_MATCH;
    do_combine_score(document);
    CHECK(node_buffer->buffer[0].roc == 100);
    free_node_score_buffer(node_buffer);
    free(document);
}

TEST(DO_COMBINE_SCORE_ROC_100_WITH_SUMMARY_MUST_HAVE, SCORE_GROUP)
{
    classifier_document *document = create_classifier_document();
    node_score_buffer *node_buffer = create_node_score_buffer(1);
    document->node_score_buffer = node_buffer;
    node_buffer->buffer[0].frequency = 2.0;
    node_buffer->buffer[0].coverage = 1.0; 
    node_buffer->buffer[0].flags = NODE_TOP;
    do_combine_score(document);
    CHECK(node_buffer->buffer[0].roc == 100);
    free_node_score_buffer(node_buffer);
    free(document);
}

TEST(DO_COMBINE_SCORE_ROC_100_WITH_SUMMARY_AND_TITLE_MUST_HAVE, SCORE_GROUP)
{
    classifier_document *document = create_classifier_document();
    node_score_buffer *node_buffer = create_node_score_buffer(1);
    document->node_score_buffer = node_buffer;
    node_buffer->buffer[0].frequency = 2.0;
    node_buffer->buffer[0].coverage = 1.0; 
    node_buffer->buffer[0].flags = TITLE_WITH_TOP;
    do_combine_score(document);
    CHECK(node_buffer->buffer[0].roc == 100);
    free_node_score_buffer(node_buffer);
    free(document);
}

static classifier_document *create_classifier_document()
{
    classifier_document *document = (classifier_document *)malloc(sizeof(classifier_document));
    return document;
}

static void free_node_score_buffer(node_score_buffer *buffer)
{
    free(buffer->buffer);
    free(buffer);
}

static node_score_buffer* create_node_score_buffer(uint32_t size)
{
    node_score_buffer *buffer = (node_score_buffer *)malloc(sizeof(node_score_buffer));
    buffer->current_size = size;
    buffer->node_score_poll_size = size;
    buffer->buffer = (node_score *)malloc(sizeof(node_score) * size);
    memset(buffer->buffer, 0, sizeof(node_score) * size);
    buffer->page = NULL;
    buffer->page_buffer = NULL;
    return buffer;
}
