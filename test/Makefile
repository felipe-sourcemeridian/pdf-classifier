CC = g++ -I../include -Iinclude -ICppUnitLite/include -ggdb
GCC = gcc -I../include -Iinclude -ICppUnitLite/include -ggdb
objects = stem_test.o score_document_test.o
LINKAGE = libcppunitlite.a
glibflags = $(shell pkg-config --libs --cflags glib-2.0)
icuflags = $(shell icu-config --cflags --ldflags --ldflags-icuio)
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
clean:
	rm test
