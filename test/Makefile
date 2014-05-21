CC = g++ -I../include -Iinclude -ICppUnitLite/include -ggdb
GCC = gcc -I../include -Iinclude -ICppUnitLite/include -ggdb
objects = stem_test.o score_document_test.o node_tax_test.o black_list_node_terms_test.o
LINKAGE = libcppunitlite.a
CFLAGS = -ggdb
glibflags = $(shell pkg-config --libs --cflags glib-2.0)
icuflags = $(shell icu-config --cflags --ldflags --ldflags-icuio)
export CFLAGS
default: $(LINKAGE) test_objects  $(objects)
	$(CC) src/main.cpp ${LINKAGE}  *.o $(glibflags) $(icuflags)  -o test
	rm *.o
test_objects:
	make -C ../ test_objects
stem_test.o:
	$(CC) -c src/stem_test.cpp
file_transformation_test.o:
	$(CC) -c src/file_transformation_test.cpp $(glibflags) $(icuflags)
score_document_test.o:
	$(CC) -c src/score_document_test.cpp $(glibflags)
node_tax_test.o:
	$(CC) -c memory_management.o src/node_tax_test.cpp $(glibflags)
black_list_node_terms_test.o:
	$(CC) -c memory_management.o src/black_list_node_terms_test.cpp $(glibflags)
clean:
	rm test
