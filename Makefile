#Makefile

SOURCES = sources
HEADERS = headers
OBJECTS = objects
SERVER = dataServer
CLIENT = remoteClient
CC = gcc
CFLAGS = -g -Wall -pedantic -pthread -I$(HEADERS)


#find all the .c files in the sources/ directory
SOURCE_FILES = $(shell find $(SOURCES)/ -name '*.c')
#e.g create from sources/file1.c  the name  objects/file1.o
OBJECT_FILES = $(SOURCE_FILES:$(SOURCES)/%.c=$(OBJECTS)/%.o)

SERVER_OBJECT = $(OBJECTS)/$(SERVER).o
CLIENT_OBJECT = $(OBJECTS)/$(CLIENT).o

############################################################################

.PHONY: clean all run valgrind

############################################################################

all: clean objects_dir $(SERVER) $(CLIENT) multiple_clients

#make the objects/ directory
objects_dir:
	mkdir -p objects


multiple_clients:
	chmod +x multiple_clients.sh

#make the dataServer programm using all the object files expect for remoteClient.o in the objects/ directory
$(SERVER): $(OBJECT_FILES)
	$(CC) $(CFLAGS) -o $(SERVER) $(filter-out $(CLIENT_OBJECT), $(OBJECT_FILES))

#make the remoteClient programm using all the object files expect for dataServer.o in the objects/ directory
$(CLIENT): $(OBJECT_FILES)
	$(CC) $(CFLAGS) -o $(CLIENT) $(filter-out $(SERVER_OBJECT), $(OBJECT_FILES))


#make the .o files
$(OBJECTS)/%.o: $(SOURCES)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

#################################################################################

run: all $(SERVER)
	./$(SERVER) -p 9000 -q 6 -s 7 -b 8192

#################################################################################

clean:
	rm -rf $(OBJECTS) SERVER_COPY_*
	rm -f $(SERVER) $(CLIENT) text*

#################################################################################

valgrind: clean all $(SERVER)
	valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./$(SERVER)


# ./remoteClient -i <server_ip> -p <server_port> -d <directory>
# ./dataServer -p <port> -s <thread_pool_size> -q <queue_size> -b <block_size>