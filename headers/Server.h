//KARAVANGELI EFTYCHIA - 1115201800062

#ifndef SERVER_H_
#define SERVER_H_

#include "libraries.h"
#include "Queue.h"

#define TOTAL_ARGS_SERVER 9

extern pthread_mutex_t mutex_files_queue;   /* mutex for Files_Queue */
extern pthread_mutex_t mutex_socket_queue;  /* mutex for Mutex_Socket_Queue */
extern pthread_mutex_t mutex_dir;           /* mutex for readdir --. it is thread-unsafe */
extern pthread_cond_t cond_queue_not_empty; /* signal worker threads when queue is not empty, make worker threads wait until queue is not empty */
extern pthread_cond_t cond_queue_not_full;  /* signal commun threads when queue is not full, make commuc threads wait until queue is not full */
extern Queue Files_Queue;                   /* keeps a file name and the socket of the client, who must receive the file */
extern Queue Mutex_Socket_Queue;            /* associates every socket number with a mutex, which gets locked when a worker
                                               thread sends a file to a client, so that no other thread can send the same 
                                               client simultaneously another file */



/* arguments for the communaction thread's start routine */
typedef struct Communication_Threads_Arguments{
   int socket;
   size_t block_size;
   int queue_size;

}Commun_Threads_Args;


/* arguments for the worker threads' (aka ThreadPool) start routine */
typedef struct Worker_Threads_Arguments{
   size_t block_size;
   int queue_size;
   int total_worker_threads;
   pthread_t* worker_threads;     /* array of worker threads of size total_worker_threads */

}Worker_Threads_Args;

extern int RUNNING;



/* signal handler to handle ^C to the server */
void Stop_Server(int);


/* Process of every CommunicationThread in the dataServer -> after a client is connected to the dataServer through a socket, 
   the server finds the requested directory and adds the files in the queue, so that the WorkerThreads remove them and send
   them to the client */
void* Server(void*);


/* Server reads the relative path of a directory name, which the client sent through the socket to the server */
char* Server_Receive_DirName_From_Client(int, char*);


/* Every communication thread of the Server: 
   1. opens, reads and extract all files from the directory requested by the 
      Client with whom server has connected through the socket
   2. inserts one by one the files in the queue until queue is full, so that worker threads can remove the files
      from the queue and send them to the right client */
void Server_Extract_Files_From_Directory(int, char*, int);


/* communication thread in the server inserts a file name with the respective socket number to the shared queue,
   by locking and unlocking the respective mutex -- waits while the queue is full */
void Server_Insert_Files_To_Queue(int, char*, int);


/* Worker thread in the Server sends to a client associated with the socket:
    1. the length of the path of the file
    2. the size of the file 
    3. the name of the file and return the name of the name of the file */
unsigned int Server_Send_FileMetaData(int, const char*);


/* Worker thread in the Server:
   1. sends the File_MetaData to a client associated with the socket
   2. reads the content of a file pre block_size from the server's filesystem
   3. sends the content of a file pre block_size to a client associated with the socket */
void Server_Send_Files_to_Client(int, const char*, size_t);  //func for threads + mutexes


/* server creates as many worker thread as specified from the command line and initializes mutexes
   and condition variables needed */
void ThreadPool_Initialize(Worker_Threads_Args*);


/* Job routine of the worker threads; after they are initialized, they run below function constantly until server closes.
   A worker: 1. removes a file from the queue and send it to the right client (through the right socket), when there 
                are files in the queue
             2. waits if there are currently no other files in the queue */
void* ThreadPool_WorkerThread_Runs(void*);


/* Servers has received ^C and must destroy all worker threads along with the mutexes and conditional variables */
void ThreadPool_Destroy(Worker_Threads_Args*);

#endif