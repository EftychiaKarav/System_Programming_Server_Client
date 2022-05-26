//KARAVANGELI EFTYCHIA - 1115201800062

#ifndef LIBRARIES_H_
#define LIBRARIES_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/select.h>
#include <errno.h>
#include <features.h>
#include <dirent.h>
#include <stdint.h>
#include <sys/socket.h>	     /* sockets */
#include <netinet/in.h>	     /* internet sockets */
#include <netdb.h>	         /* gethostbyaddr */
#include <arpa/inet.h>
#include <pthread.h>


#define TOTAL_ARGS_SERVER 9
#define TOTAL_ARGS_CLIENT 7
#define MAX_LENGTH 4096
//#define DEFAULT_DIR "Documents"
#define DEFAULT_DIR "/mnt/d/DOCUMENTS/"
#define WRONG_MSG "WRONG DIRECTORY NAME"
#define ACK_MSG "ACK"
#define FILES_SENT_MSG "TOTAL FILES TO SEND: "
#define OUT_DIR "SERVER_COPY_" 
#define TERMINATION_MSG "ALL FILES WERE TRANSFERED"

extern int RUNNING;


typedef struct Communication_Threads_Arguments{
   int socket;
   size_t block_size;

}Commun_Threads_Args;



void Print_Error(char*);
void Print_Error_Value(char*, int);
void Clear_Buffer(char*, int);
void* Server_Job(void*);
void Client(int, char*);
void Extract_Files_From_Directory(int, char*);  //maybe void 
void Send_Files_to_Client(size_t);
int Resolve_FilePath(char*, char*);


#endif