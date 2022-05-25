//KARAVANGELI EFTYCHIA - 1115201800062

#include "../headers/Queue.h"

struct Queue_Node{
    
    char* fileName;    //either name of the FIFO for the worker, or the name of the link location for the links
    unsigned int socket; //either pid for worker or number of occurences for a link
    QNode next;
};

Queue Files_Queue = NULL;

/**************************************************************************************************/

QNode QueueNode_Create_Node(unsigned int socket, char* filename_path){
    
    QNode queue_node = malloc(sizeof(struct Queue_Node));
    if (queue_node == NULL){
        return NULL;    //se error kati na valo
    }
    queue_node->socket = socket;
    queue_node->fileName = (char*)calloc(strlen(filename_path)+1, sizeof(char));
    strcpy(queue_node->fileName, filename_path);
    queue_node->next = NULL;
    //printf("%s, %ld\n", queue_node->FIFO_name, strlen(queue_node->FIFO_name));

    //free(LinkName);
    return queue_node;

}


unsigned int QueueNode_GetSocket(QNode queue_node){
    return queue_node->socket;
}

char* QueueNode_GetFileName(QNode q_node){
    return q_node->fileName;
}

QNode QueueNode_Next(QNode q_node){
    return q_node->next;
}

void QueueNode_Delete(QNode q_node){

    free(q_node->fileName);
    free(q_node);
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
        return NULL;    //se error kati na valo
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
        QNode node = Queue_Pop(queue);
        QueueNode_Delete(node);
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
    // if (queue->size == 1){
    //     queue->last = queue->first;
    // }
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

void Queue_Print(const Queue queue){

    QNode q_node = queue->first;
    int i = 1;
    while(q_node != NULL){
        printf("i = %d\t, socket_fd = %d\t, file = %s\n", i, q_node->socket, q_node->fileName);
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