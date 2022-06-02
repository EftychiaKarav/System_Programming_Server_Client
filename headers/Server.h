//KARAVANGELI EFTYCHIA - 1115201800062

#ifndef SERVER_H_
#define SERVER_H_

#include "libraries.h"
#include "Queue.h"

#define TOTAL_ARGS_SERVER 9

extern pthread_mutex_t mutex_files_queue;
extern pthread_mutex_t mutex_socket_queue;
extern pthread_mutex_t mutex_dir;
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

extern int RUNNING;

/* Process of every CommunicationThread in the dataServer -> after a client is connected to the dataServer through a socket, 
   the server finds the requested directory and adds the files in the queue, so that the WorkerThreads remove them and send
   them to the client */
void* Server(void*);


/* Server reads the relative path of a directory name, which the client sent through the socket to the server */
char* Server_Receive_DirName_From_Client(int, char*);



void Server_Extract_Files_From_Directory(int, char*, int);  //maybe void 
void Server_Send_Files_to_Client(int, const char*, size_t);  //func for threads + mutexes
void Stop_Server(int);

void ThreadPool_Initialize(Worker_Threads_Args*);
void ThreadPool_Destroy(Worker_Threads_Args*);
void* ThreadPool_WorkerThread_Runs(void*);
#endif