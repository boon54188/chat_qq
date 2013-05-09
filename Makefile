TARGET = server  #client
CFLAGS = -g
CC = gcc -lpthread

objects = server.o check_online.o#client.o 

server: $(objects)
	$(CC) -o server $(objects)

ALL:$(TARGET)

server.o: server.h check_online.c
	$(CC) $(CFLAGS) -c server.c check_online.c


#client.o:
#	$(CC) $(CFLAGS) -c client.c
	

.PHONY : clean
clean:
	rm -rf  $(objects)
	rm -rf $(TARGET)
