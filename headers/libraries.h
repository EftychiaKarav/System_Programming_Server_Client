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
#define TERM_MSG "END"
#define CONFIRMATION_MSG "About to scan directory: "
#define OUT_DIR "SERVER_COPY_" 
#define TERMINATION_MSG "ALL FILES WERE TRANSFERED"

extern int RUNNING;
extern pthread_mutex_t mutex_files_queue;
extern pthread_mutex_t mutex_socket_queue;
extern pthread_cond_t cond_queue_not_empty;
extern pthread_cond_t cond_queue_not_full;


typedef struct Communication_Threads_Arguments{
   int socket;
   size_t block_size;
   int queue_size;

}Commun_Threads_Args;

typedef struct Worker_Threads_Arguments{
   size_t block_size;
   int queue_size;
   int total_worker_threads;
   pthread_t* worker_threads;

}Worker_Threads_Args;

void Stop_Server(int);

void Print_Error(char*);
void Print_Error_Value(char*, int);
void Clear_Buffer(char*, int);
void* Server_Job(void*);
void Client(int, char*);
void Extract_Files_From_Directory(int, char*, int);  //maybe void 
int Resolve_FilePath(char*, char*);
void Send_Files_to_Client(int, const char*, size_t);  //func for threads + mutexes

void ThreadPool_Initialize(Worker_Threads_Args*);
void ThreadPool_Destroy(Worker_Threads_Args*);
void* ThreadPool_WorkerThread_Runs(void*);
#endif