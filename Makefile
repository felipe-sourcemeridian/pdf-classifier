OS := $(shell uname -o)
glibflags = $(shell pkg-config --libs --cflags glib-2.0)
icuflags = $(shell icu-config --cflags --ldflags --ldflags-icuio)
INCLUDE = include
SRC = src
CC = gcc -Wall -I $(INCLUDE)
OBJS = trans.o porter.o memory_management.o memory_poll.o  file_transformation.o load_files.o classifier.o state_aho.o score_document.o classifier_utils.o utility.o file_server.o daemon_util.o config_util.o request_management.o
default: $(OBJS) 
	$(CC) $(OBJS) $(SRC)/main.c -o classifier $(glibflags) $(icuflags) -std=gnu99
	rm $(OBJS)
trans.o: $(SRC)/trans.c $(INCLUDE)/trans.h
	$(CC) -c $(SRC)/trans.c
memory_management.o: $(INCLUDE)/memory_management.h $(SRC)/memory_management.c
	$(CC) -c $(SRC)/memory_management.c 
memory_poll.o: memory_management.o $(INCLUDE)/memory_poll.h $(SRC)/memory_poll.c
	$(CC) -c $(SRC)/memory_poll.c
porter.o: $(SRC)/porter.c $(INCLUDE)/porter.h
	$(CC) -c $(SRC)/porter.c
file_transformation.o: $(SRC)/file_transformation.c $(INCLUDE)/file_transformation.h
	$(CC) -c $(SRC)/file_transformation.c $(glibflags)
load_files.o: $(SRC)/load_files.c $(INCLUDE)/load_files.h
	$(CC) -c $(SRC)/load_files.c $(glibflags)
state_aho.o: $(INCLUDE)/state_aho.h $(SRC)/state_aho.c
	$(CC) -c $(SRC)/state_aho.c $(glibflags)
classifier.o: state_aho.o memory_poll.o
	$(CC) -c $(SRC)/classifier.c -c $(glibflags)
classifier_utils.o: $(INCLUDE)/memory_management.h $(SRC)/memory_management.c
	$(CC)  -c $(SRC)/classifier_utils.c  $(glibflags)
file_server.o: $(INCLUDE)/file_server.h $(SRC)/file_server.c
	$(CC) -c $(SRC)/file_server.c $(glibflags)
utility.o: config_util.o classifier.o state_aho.o $(INCLUDE)/utility.h
	$(CC) -c  $(SRC)/utility.c $(glibflags)
daemon_util.o: $(INCLUDE)/daemon_util.h $(SRC)/daemon_util.c
	$(CC) -c $(SRC)/daemon_util.c
score_document.o: $(INCLUDE)/load_files.h memory_poll.o $(INCLUDE)/score_document.h $(SRC)/score_document.c
	$(CC) -c $(SRC)/score_document.c  $(glibflags)
config_util.o: $(INCLUDE)/config_util.h $(SRC)/config_util.c
	$(CC) -c $(SRC)/config_util.c $(glibflags)
request_management.o: $(INCLUDE)/request_management.h $(SRC)/request_management.c
	$(CC) -c $(SRC)/request_management.c $(glibflags) $(icuflags)
clean:
	rm classifier
