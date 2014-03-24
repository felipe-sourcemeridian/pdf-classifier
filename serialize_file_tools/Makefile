CC = gcc -Wall  -I include -I ../include
objs = memory_page_manager.o
default:$(objs)
	$(CC) src/main.c $(objs) -o serialize_state_aho
	rm *.o
memory_page_manager.o:
	$(CC) -c src/memory_page_manager.c
state_aho_poll.o:
	$(CC) -c src/state_aho_poll.c
