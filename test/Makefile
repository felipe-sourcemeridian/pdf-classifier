CC = g++ -I../include -Iinclude -ICppUnitLite/include
objects = stem_test.o
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
