//KARAVANGELI EFTYCHIA - 1115201800062

#ifndef CLIENT_H_
#define CLIENT_H_
#include "libraries.h"
#include "Queue.h"

#define TOTAL_ARGS_CLIENT 7
#define OUT_DIR "SERVER_COPY_"   /* directory name format for the directory where the copied files will be stored */

/* Process of the remoteClient -> the client is connected to the dataServer through a socket
   and asks for a directory from the server in order to copy it in its file system*/
void Client(int, char*);


/* Client recreates the same file system hierarchy as the server does into a new directory with name:
   "SERVER_COPY_[pid_Client]", so that every client can have what they asked in a seperate directory  */
int Client_Resolve_FilePath(char*, char*);


/* Read 1. the length of the path of the file
        2. the size of the file 
        3. the name of the file and return the name of the name of the file*/
char* Client_Get_FileMetaData(int, char*, uint32_t*);


/* Client 1. creates the [output_dir] which will contain the file system of the Server, which Client asked
          2. receives the files and copies them in the directory, after he creates the right hierarchy */
void Client_CopyFiles(int, char*, size_t);


#endif