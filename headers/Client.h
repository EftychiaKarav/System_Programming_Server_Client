//KARAVANGELI EFTYCHIA - 1115201800062

#ifndef CLIENT_H_
#define CLIENT_H_
#include "libraries.h"
#include "Queue.h"

#define TOTAL_ARGS_CLIENT 7
#define OUT_DIR "SERVER_COPY_" 

/* Process of the remoteClient -> the client is connected to the dataServer through a socket
   and asks for a directory from the server in order to copy it in its file system*/
void Client(int, char*);

/* Client recreates the same file system hierarchy as the server does into a new directory with name:
   "SERVER_COPY_[pid_Client]", so that every client can have what they asked in a seperate directory  */
int Resolve_FilePath(char*, char*);

#endif