#AM: 1115201800062
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

#make the sniffer programm using all the object files in the objects/ directory
$(SERVER): $(OBJECT_FILES)
	$(CC) $(CFLAGS) -o $(SERVER) $(filter-out $(CLIENT_OBJECT), $(OBJECT_FILES))


$(CLIENT): $(OBJECT_FILES)
	$(CC) $(CFLAGS) -o $(CLIENT) $(filter-out $(SERVER_OBJECT), $(OBJECT_FILES))


#make the .o files
$(OBJECTS)/%.o: $(SOURCES)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

#################################################################################

run: all $(SERVER)
	./$(SERVER) 

#################################################################################

clean:
	rm -rf $(OBJECTS) *COPY*
	rm -f $(SERVER) $(CLIENT) text*

#################################################################################

valgrind1: clean all $(SERVER)
	valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes --trace-children=yes ./$(SERVER)


# commands:
# 	sudo umount /mnt/d
# 	sudo mount -t drvfs D: /mnt/d -o metadata

# ./remoteClient -i 127.0.0.1 -p 8000 -d ../SEMESTER_6/SYSPRO/System_Programming_Exec1_Sniffer
# ./dataServer -p 9000 -q 8 -s 6 -b 1024
# /mnt/d/SEMESTER_6/SYSPRO/System_Programming_Exec2_Server_Client
# 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