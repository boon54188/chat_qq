TARGET = server  #client
CFLAGS = -g
CC = gcc -lpthread

DIR = file/file.c


objects = server.o check_online.o get_sockfd.o subnet.o file/file.o list.o udp_handle.o#client.o 

server: $(objects) $(DIR)
	$(CC) -o server $(objects) 

ALL:$(TARGET)

server.o: server.h 
	$(CC) $(CFLAGS) -c server.c 
check_online.o: server.h
	$(CC) $(CFLAGS) -c check_online.c 
get_sockfd.o: server.h
	$(CC) $(CFLAGS) -c get_sockfd.c
subnet.o: server.h
	$(CC) $(CFLAGS) -c subnet.c
file.o: server.h
	$(CC) $(CFLAGS) -c $(DIR)


list.o: server.h
	$(CC) $(CFLAGS) -c list.c
udp_handle.o: server.h
	$(CC) $(CFLAGS) -c udp_handle.c


#client.o:
#	$(CC) $(CFLAGS) -c client.c
	

.PHONY : clean
clean:
	rm -rf  $(objects)
	rm -rf $(TARGET)
