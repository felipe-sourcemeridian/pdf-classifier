OS := $(shell uname -o)
glibflags = $(shell pkg-config --libs --cflags glib-2.0)
icuflags = $(shell icu-config --cflags --ldflags --ldflags-icuio)
INCLUDE = include
SRC = src
CC = gcc -Wall -I $(INCLUDE)
OBJS = trans.o porter.o memory_management.o memory_poll.o  file_transformation.o load_files.o classifier.o state_aho.o score_document.o classifier_utils.o utility.o  daemon_util.o config_util.o fileserver.o parse.o client_reader.o request_manager_builder.o request_manager.o
OBJS_LINKAPEDIA_SERVER = fileserver.o parse.o client_reader.o request_manager_builder.o request_manager.o memory_poll.o memory_management.o
default: $(OBJS) 
	$(CC) $(OBJS) $(SRC)/main.c -o classifier $(glibflags) $(icuflags) -std=gnu99
	rm $(OBJS)
linkapediaserver: $(OBJS_LINKAPEDIA_SERVER)
	$(CC) $(OBJS_LINKAPEDIA_SERVER) $(SRC)/linkapediaserver.c -o linkapediaserver $(glibflags) $(icuflags) 
	rm $(OBJS_LINKAPEDIA_SERVER)
baseserver: fileserver.o
	$(CC) fileserver.o $(SRC)/baseserver.c -o baseserver
	rm fileserver.o
trans.o: $(SRC)/trans.c $(INCLUDE)/trans.h
	$(CC) -c $(SRC)/trans.c
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
utility.o: config_util.o classifier.o state_aho.o $(INCLUDE)/utility.h
	$(CC) -c  $(SRC)/utility.c $(glibflags)
daemon_util.o: $(INCLUDE)/daemon_util.h $(SRC)/daemon_util.c
	$(CC) -c $(SRC)/daemon_util.c
score_document.o: $(INCLUDE)/load_files.h memory_poll.o $(INCLUDE)/score_document.h $(SRC)/score_document.c
	$(CC) -c $(SRC)/score_document.c  $(glibflags)
config_util.o: $(INCLUDE)/config_util.h $(SRC)/config_util.c
	$(CC) -c $(SRC)/config_util.c $(glibflags)
fileserver.o: $(INCLUDE)/fileserver.h
	$(CC) -c $(SRC)/fileserver.c -o fileserver.o

parse.o: $(INCLUDE)/protocol.h $(INCLUDE)/classify_document_request_parse.h
	$(CC) -c $(SRC)/classify_document_request_parse.c -o parse.o

client_reader.o: $(INCLUDE)/client_reader.h
	$(CC) -c $(SRC)/client_reader.c -o client_reader.o
request_manager_builder.o: $(INCLUDE)/request_manager_builder.h $(INCLUDE)/request_manager.h
	$(CC) -c $(SRC)/request_manager_builder.c -o request_manager_builder.o
request_manager.o:
	$(CC) -c $(SRC)/request_manager.c -o request_manager.o
memory_management.o: $(INCLUDE)/memory_management.h $(SRC)/memory_management.c
	$(CC) -c $(SRC)/memory_management.c -o memory_management.o

clean:
	rm classifier
