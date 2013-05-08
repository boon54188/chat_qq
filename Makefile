CFLAGS = -g
CC = gcc -lpthread
objects = server.o #client.o
TARGET = server #client

ALL:$(TARGET)

server.o: server.h
	$(CC) $(CFLAGS) -c server.c
#client.o:
#	$(CC) $(CFLAGS) -c client.c
	

.PHONY : clean
clean:
	rm -rf  $(objects)
	rm -rf $(TARGET)
