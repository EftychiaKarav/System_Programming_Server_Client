//KARAVANGELI EFTYCHIA - 1115201800062

#include "../headers/Queue.h"


union Data_Node{
    char* fileName;                  /* for Files_Queue */
    pthread_mutex_t mutex_socket;    /* for Mutex_Socket_Queue */
};


struct Queue_Node{
    
    unsigned int socket; /* socket number */
    Data data;           /* either filename or a mutex */
    QNode next;          /* pointer to the next node */
};

Queue Files_Queue = NULL;
Queue Mutex_Socket_Queue = NULL;

/**************************************************************************************************/

/* methods for Datatype Queue_Node -> QNode is a pointer to Queue_Node */

QNode QueueNode_Create_Node(unsigned int socket, void* data){
    
    QNode queue_node = (QNode)calloc(1, sizeof(struct Queue_Node));
    Data data_node = (Data)calloc(1, sizeof(union Data_Node));
    if ((queue_node == NULL) || (data_node == NULL)){
        return NULL;    
    }
    queue_node->socket = socket;
    queue_node->data = data_node;
    if(data != NULL){     /* it is a node for the Files_Queue */
        char* filename_path = (char*)data;
        queue_node->data->fileName = (char*)calloc(strlen(filename_path)+1, sizeof(char));
        strcpy(queue_node->data->fileName, filename_path);
        queue_node->next = NULL;

    }
    else{    /* it is a node for the Mutex_Socket_Queue */
        pthread_mutex_init(&queue_node->data->mutex_socket, NULL);
    }
    return queue_node;

}

/* releases memory only -- does not disconnect nodes */
void QueueNode_Delete(QNode q_node){ 

    if(q_node->data->fileName != NULL){
        free(q_node->data->fileName);
    }
    else{
        pthread_mutex_destroy(&q_node->data->mutex_socket);
    }
    free(q_node->data);
    free(q_node);
}



/**************************************************************************************************/


/* methods for the attributes of the Queue */

/* lock the mutex for a specific socket */
void QueueNode_LockMutex(QNode qnode){
    pthread_mutex_lock(&qnode->data->mutex_socket);
}

/* unlock the mutex for a specific socket */
void QueueNode_UnlockMutex(QNode qnode){
    pthread_mutex_unlock(&qnode->data->mutex_socket);
}


unsigned int QueueNode_GetSocket(QNode queue_node){
    return queue_node->socket;
}

char* QueueNode_GetFileName(QNode q_node){
    return q_node->data->fileName;
}

pthread_mutex_t QueueNode_GetMutex(QNode q_node){
    return q_node->data->mutex_socket;
}

QNode QueueNode_Next(QNode q_node){
    return q_node->next;
}



/**************************************************************************************************/

//   Queue Methods

struct Queue{
    QNode first;
    QNode last;
    unsigned int size;
};


Queue Queue_Initialize(){

    Queue queue = malloc(sizeof(struct Queue));
    if (queue == NULL){
        return NULL;    
    }
    queue->first = NULL;
    queue->last = NULL;
    queue->size = 0;

    return queue;
}

bool Queue_Exists(const Queue queue){
    return (queue != NULL) ? true : false; 
}

bool Queue_isEmpty(const Queue queue){
    
    return (queue->size == 0) ? true : false;
}


void Queue_Insert(Queue queue, QNode queue_node){

    if (Queue_isEmpty(queue)){
        queue->first = queue_node;
    }
    else{
        queue->last->next = queue_node;
    }
    queue->last = queue_node;
    queue->size++;

}


void Queue_Destroy(Queue queue){

    while (queue->first != NULL && queue->size != 0){
        QNode node = Queue_Pop(queue);   /* disconnects node from queue */
        QueueNode_Delete(node);          /* release memory */
    }
    free(queue);

}

QNode Queue_Pop(Queue queue){

    if(Queue_isEmpty(queue)){
        return NULL;
    }
    QNode node_to_be_popped = queue->first;
    queue->first = queue->first->next;
    queue->size--;
    return node_to_be_popped;
    
}

QNode Queue_Find(Queue queue, unsigned int socket_fd){

    QNode q_node = queue->first;
    while(q_node != NULL){
        if(q_node->socket == socket_fd){
            break;
        }
        q_node = q_node->next;
    }
    return q_node;
}

void Queue_Delete(Queue queue, QNode q_node){

    if(q_node == queue->first){
        q_node = Queue_Pop(queue);
        QueueNode_Delete(q_node);
    }
    else{            /* find node and disconnect it from queue */
        QNode prev_node = queue->first;
        QNode node_to_delete = queue->first->next;
        while(node_to_delete != q_node){
            prev_node = node_to_delete;
            node_to_delete = node_to_delete->next;
        }
        prev_node->next = node_to_delete->next;
        QueueNode_Delete(q_node);
        queue->size--;
    }

}

void Queue_Print(const Queue queue){

    QNode q_node = queue->first;
    int i = 1;
    while(q_node != NULL){
        printf("i = %d\t, socket_fd = %d\t, file = %s\n", i, q_node->socket, q_node->data->fileName);
        q_node = q_node->next;
        i++;
    }
    printf("Queue Size = %d\n", queue->size);
}


QNode Queue_First(Queue queue){
    return queue->first;
}

QNode Queue_Last(Queue queue){
    return queue->last;
}


unsigned int Queue_Size(Queue queue){
    return queue->size;
}