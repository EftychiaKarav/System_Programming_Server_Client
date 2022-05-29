//KARAVANGELI EFTYCHIA - 1115201800062

#ifndef QUEUE_H_
#define QUEUE_H_

#include "libraries.h"

/**************************************************************************************************/

typedef struct Queue_Node*       QNode;
typedef struct Queue*            Queue;
typedef union Data_Node*         Data;

/**************************************************************************************************/

//  methods for Datatype Queue_Node -> a QNode in our case is either a link or a worker 
QNode QueueNode_Create_Node(unsigned int, void*);
unsigned int QueueNode_GetSocket(QNode);
char* QueueNode_GetFileName(QNode);
pthread_mutex_t QueueNode_GetMutex(QNode);
void QueueNode_LockMutex(QNode);
void QueueNode_UnlockMutex(QNode);
QNode QueueNode_Next(QNode);
void QueueNode_Delete(QNode);


//  methods for Queue
Queue Queue_Initialize();
bool Queue_isEmpty(const Queue);
bool Queue_Exists(const Queue);
QNode Queue_Pop(Queue);
QNode Queue_First(Queue);
QNode Queue_Last(Queue);  
unsigned int Queue_Size(Queue);
void Queue_Destroy(Queue);  
void Queue_Insert(Queue, QNode);
void Queue_Delete(Queue, QNode);
QNode Queue_Find(Queue, unsigned int);
void Queue_Print(const Queue);


extern Queue Files_Queue;
extern Queue Mutex_Socket_Queue;
#endif