CFLAGS = -g
CC = gcc -lpthread
objects = server.o client.o
TARGET = server client

ALL:$(TARGET)

server.o:
	$(CC) $(CFLAGS) -c server.c
client.o:
	$(CC) $(CFLAGS) -c client.c
	

.PHONY : clean
clean:
	rm -rf  $(objects)
	rm -rf $(TARGET)
