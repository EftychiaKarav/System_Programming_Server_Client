//KARAVANGELI EFTYCHIA - 1115201800062

#include "../headers/Server.h"

pthread_mutex_t mutex_files_queue;
pthread_mutex_t mutex_socket_queue;
pthread_mutex_t mutex_dir;
pthread_cond_t cond_queue_not_empty;
pthread_cond_t cond_queue_not_full;
int RUNNING = 1;


/* signal handler to handle ^C to the server */
void Stop_Server(int sugnum){
    printf("\n\nSERVER GOT ^C, exiting in a while...\n");
    RUNNING = 0;
}

/* Job routine of every CommunicationThread in the dataServer -> after a client is connected to the dataServer through a
   socket, the server finds the requested directory and adds the files in the queue, so that the WorkerThreads remove 
   them and send them to the client */

void* Server(void* arguments){

    Commun_Threads_Args args = *(Commun_Threads_Args*)arguments;
    int socket = args.socket;    /* socket so that server - client can communicate */
    size_t block_size = args.block_size;
    int max_queue_size = args.queue_size;
    
    /* create and add the pair <socket, mutex> to the Mutex_Socket_Queue */
    pthread_mutex_lock(&mutex_socket_queue);
    QNode q_node = QueueNode_Create_Node(socket, NULL);
    Queue_Insert(Mutex_Socket_Queue, q_node);
    pthread_mutex_unlock(&mutex_socket_queue);

    int err;
    /* Detach thread --> when a client gets all files, the respective communication thread can terminate and exit
       without having to wait for the other communication threads to terminate  */
    if ((err = pthread_detach(pthread_self())) != 0) {    
       Print_Error_Value("Error in pthread_detach", err);
    }
    char buffer[MAX_LENGTH] = {'\0'};

    /* 1. Receive the path to the directory, which is to be sent to the client */
    char* path = Server_Receive_DirName_From_Client(socket, buffer);

    /* 2. Receive confirmation from client */
    Receive_Data(socket, buffer, strlen(ACK_MSG), "SERVER: Read ACK message from client");
    
    /* 3. Send block_size to client */
    uint32_t bl_size = htonl(block_size);  /* convert from host order to Network Byte Order */
    Send_Data(socket, &bl_size, sizeof(uint32_t), "SERVER: WRITE block size");
    
    /* 4. Receive confirmation from client */
    Receive_Data(socket, buffer, strlen(ACK_MSG), "SERVER: Read ACK message from client");

    /* 5. Read requested directory and insert the file names to the Files_Queue -- wait if queue is full */
    Server_Extract_Files_From_Directory(socket, path, max_queue_size);
    free(path);

    /* 6. Insert  termination message to the Files_Queue after having inserted all the other file names */
    Server_Insert_Files_To_Queue(socket, NULL, max_queue_size);

    /* 7. server reads "ACK" from client until he reads an "END" --> then terminates */
    int FINISHED = 0;
    while(!FINISHED){
        
        /* receives a message */
        Receive_Data(socket, buffer, strlen(ACK_MSG), "SERVER: Read ACK message from client");
        
        if (strncmp(buffer, TERM_MSG, strlen(TERM_MSG)) == 0){      /* TERM MSG */
            printf("[Communication Thread: %ld] ---> Finished\n", pthread_self());
            FINISHED = 1;
            Clear_Buffer(buffer, MAX_LENGTH);
        }            
        else if(strncmp(buffer, ACK_MSG, strlen(ACK_MSG)) == 0){   /* ACK MSG */
            Clear_Buffer(buffer, MAX_LENGTH);
        }

    }

    /* 8. delete socket and mutex from the Mutex_Socket_Queue -- this socket will close,
          since client has received all files */
    pthread_mutex_lock(&mutex_socket_queue);
    Queue_Delete(Mutex_Socket_Queue, q_node);
    pthread_mutex_unlock(&mutex_socket_queue);

    /* 9. close socket connection with the client */
    if(close(socket) == -1){
        Print_Error("SERVER: Close new socket");
    }

    free(arguments);
    printf("[Communication Thread: %ld] ---> Exits\n", pthread_self());
    pthread_exit(NULL);

}

/******************************************************************************************************************/



/* Server reads the relative path of a directory name, which the client sent through the socket to the server */
char* Server_Receive_DirName_From_Client(int socket, char* buffer){

    uint16_t dir_length = 0;
    /* 1. Server receives the length of the relative path of a directory name from the client through the socket */
    Receive_Data(socket, &dir_length, sizeof(uint16_t),"SERVER: Read directory length");
    dir_length = ntohs(dir_length);   //convert from Network Byte Order to host order

    /* 2. Server receives the relative path of a directory name from the client through the socket */
    Receive_Data(socket, buffer, dir_length,"SERVER: Read directory name");

    /* 3. Server builds the path of the directory to open; it is relevant to the [DEFAULT_DIR] */
    char* path = (char*)calloc(strlen(DEFAULT_DIR) + strlen(buffer) + 1, sizeof(char));
    snprintf(path, strlen(DEFAULT_DIR) + strlen(buffer) + 1, "%s%s%c", DEFAULT_DIR, buffer, '\0');
    Clear_Buffer(buffer, MAX_LENGTH);
    
    DIR* dir_ptr = NULL;
    /* 4. Server opens directory to verify it is a valid directory; if not, the communication thread exits */
    if((dir_ptr = opendir(path)) == NULL ){
        fprintf(stdout, "Cannot open %s directory\n", path);
        Clear_Buffer(buffer, MAX_LENGTH);
        Send_Data(socket, WRONG_MSG, strlen(WRONG_MSG), "SERVER: WRITE \"WRONG DIR NAME\" ");
        printf("THREAD %ld is exiting\n", pthread_self());
        int error = 1;
        pthread_exit(&error);
    }
    /* 5. Server closes the above opened directory */
    if(closedir(dir_ptr) < 0 ){
        fprintf(stdout, "Cannot close %s directory\n", path);
        exit(EXIT_FAILURE);
    }

    /* 6. server sends confirmation to the client, that his request is received */
    char* mess = (char*)calloc(strlen(CONFIRMATION_MSG) + strlen(path) + 1, sizeof(char));
    snprintf(mess, strlen(CONFIRMATION_MSG) + strlen(path) + 1, "%s%s%c", CONFIRMATION_MSG, path, '\0');
    printf("[Communication Thread: %ld] ---> %s\n", pthread_self(), mess);
    
    /* 7. server sends the length of the directory path in his file system to the client */
    dir_length = htons(strlen(mess));     /* convert from host order to Network Byte Order */
    Send_Data(socket, &dir_length, sizeof(uint16_t), "SERVER: WRITE CONFIRM_MSG length");
    
    /* 8. server sends the name of the directory path in his file system to the client */
    Send_Data(socket, mess, strlen(mess), "SERVER: WRITE \"ABOUT TO SCAN DIR:\" ");
    free(mess);

    return path;    /*path to the directory name to send to client */

}



/******************************************************************************************************************/


/* Every communication thread of the Server: 
   1. opens, reads and extract all files from the directory requested by the 
      Client with whom server has connected through the socket
   2. inserts one by one the files in the queue until queue is full, so that worker threads can remove the files
      from the queue and send them to the right client */
void Server_Extract_Files_From_Directory(int socket, char* path, int max_queue_size){

    /* Save the type of the node; directory or regular file */
    struct stat node;
    memset(&node, 0, sizeof(struct stat));

    /* 1. get node status */
    if(stat(path, &node) < 0 ){
        Print_Error("SERVER: Stat while reading directory");
    }

    /* 2. test against the S_IFMT flag to get the first 4 bits of the node to determine the file type */
    mode_t outcome = node.st_mode & S_IFMT;   
    
    if (outcome == S_IFDIR){    /* directory */
        
        DIR* dir_ptr = NULL;                 // pointer to an opened directory
        struct 	dirent *dir_entity = NULL;   // struct for items in the directory

        /* 3. Open directory */
        if((dir_ptr = opendir(path)) == NULL ){
            fprintf(stdout, "Cannot open %s directory\n", path);
            exit(EXIT_FAILURE);
        }

        /* 4. keep the current path */
        char* curr_path = (char*)calloc(strlen(path)+ 1 + 1, sizeof(char));   //1 for '/' one for '\0'
        memcpy(curr_path, path, strlen(path));
        memcpy(curr_path + strlen(path), "/", 1);

        /* 5. Read directory -- readdir non-thread-safe */
        pthread_mutex_lock(&mutex_dir);
        dir_entity = readdir(dir_ptr);
        pthread_mutex_unlock(&mutex_dir);

        
        while ( dir_entity != NULL ){
            
            /* if the directory is not cwd or pwd */
            if((strcmp(dir_entity->d_name, ".") != 0) && (strcmp(dir_entity->d_name, "..") != 0)){

                /* 6. add the new item to the path */
                char* final_path = (char*)calloc(strlen(curr_path)+ strlen(dir_entity->d_name) + 1, sizeof(char));
                memcpy(final_path, curr_path, strlen(curr_path));
                memcpy(final_path + strlen(curr_path), dir_entity->d_name, strlen(dir_entity->d_name));
                
                /* 7. Recursion: input is the new path which is the path to the new directory;
                      the function is called again to go inside the new directory and read its items */
                Server_Extract_Files_From_Directory(socket, final_path, max_queue_size);

                free(final_path);
            }
            /*8. read the next item in the directory */
            pthread_mutex_lock(&mutex_dir);
            dir_entity = readdir(dir_ptr);
            pthread_mutex_unlock(&mutex_dir);
        }
        
        /* delete current path */
        free(curr_path);

        /* Close directory */
        if(closedir(dir_ptr) < 0 ){
            fprintf(stdout, "Cannot close %s directory\n", path);
            exit(EXIT_FAILURE);
        }
        
    }
    else if(outcome == S_IFREG){    /* regular file */
        
        /* communication thread in the server inserts a file name with the respective socket number to the shared
           queue, by locking and unlocking the respective mutex -- waits while the queue is full */
        Server_Insert_Files_To_Queue(socket, path, max_queue_size);
    }

    /* return if item is a regular file */
    return;
}


/*********************************************************************************************************************/


/* communication thread in the server inserts a file name with the respective socket number to the shared
   queue, by locking and unlocking the respective mutex -- waits while the queue is full */
void Server_Insert_Files_To_Queue(int socket, char* path, int max_queue_size){
    
    QNode node = NULL;
    if (path == NULL){       /* termination message */
        node = QueueNode_Create_Node(socket, TERMINATION_MSG);
    }
    else{    /* file name with path */
        node = QueueNode_Create_Node(socket, path);
    }
    pthread_mutex_lock(&mutex_files_queue);                            // lock mutex
    while(Queue_Size(Files_Queue) >= max_queue_size){                  // wait if queue while queue is full 
        pthread_cond_wait(&cond_queue_not_full, &mutex_files_queue);
    }
    if (path != NULL){
        printf("[Communication Thread: %ld] ---> ADDING FILE < %s > to the queue...\n", pthread_self(), path);
    }
    Queue_Insert(Files_Queue, node);                                   // insert file - socket node to the queue
    pthread_mutex_unlock(&mutex_files_queue);                          // unlock mutex

    pthread_cond_signal(&cond_queue_not_empty);   // notify worker threads to start removing file names from Files_Queue

    return;
}


/*********************************************************************************************************************/


/* Worker thread in the Server sends to a client associated with the socket:
    1. the length of the path of the file
    2. the size of the file 
    3. the name of the file and return the name of the name of the file */
unsigned int Server_Send_FileMetaData(int socket, const char* path_to_file){
    
    uint16_t file_length = 0;
    uint32_t file_size = 0;
    struct stat file_info;
    memset(&file_info, '\0', sizeof(file_info));

    /* 1. find the size of the file */
    if(stat(path_to_file, &file_info) < 0){
        Print_Error("SERVER: Could not get info for the file to process");
    }

    /* 2. send to a client associated with a socket the length of the path of the file*/
    file_length = htons(strlen(path_to_file));     /* convert from host order to Network Byte Order */
    Send_Data(socket, &file_length, sizeof(uint16_t), "SERVER: WRITE file length");

    /* 3. send to a client associated with a socket the size of the file*/
    file_size = htonl(file_info.st_size);          /* convert from host order to Network Byte Order */
    Send_Data(socket, &file_size, sizeof(uint32_t), "SERVER: WRITE file size");
    
    /* 4. send to a client associated with a socket the path_name of the file*/
    Send_Data(socket, (char*)path_to_file, strlen(path_to_file), "SERVER: WRITE file name");
    
    return (uint32_t)file_info.st_size;
}


/*********************************************************************************************************************/


/* Worker thread in the Server:
   1. sends the File_MetaData to a client associated with the socket
   2. reads the content of a file pre block_size from the server's filesystem
   3. sends the content of a file pre block_size to a client associated with the socket */
void Server_Send_Files_to_Client(int socket, const char* path_to_file, size_t block_size){   //func for threads + mutexes

    int file_fd = -1;                                            /* file desc for the file to read the content from */
    uint32_t file_size = 0;
    char* buffer = (char*)calloc(block_size + 1, sizeof(char));  /* buffer to send the content */  
    
    if(strcmp(path_to_file, TERMINATION_MSG) != 0){
        printf("[Worker Thread: %ld]  ----> RECEIVED TASK < %s, %d > \n", pthread_self(), path_to_file, socket);
        
        /* 1. SEND FILE METADATA */
        file_size = Server_Send_FileMetaData(socket, path_to_file);

        /* 2. Open file to read the content from */
        while ((file_fd = open(path_to_file, O_RDONLY)) < 0){
            Print_Error("SERVER: Could not open file to read");
        }

        /* 3. SEND FILE CONTENT */
        unsigned int bytes_to_send = file_size;
        printf("[Worker Thread: %ld]  ----> ABOUT TO READ FILE < %s >\n", pthread_self(), path_to_file);
        while(bytes_to_send){

            if(bytes_to_send < block_size)   
                block_size = bytes_to_send;
            
            /* 3. Read content from the file in his file system block size by block size */
            Receive_Data(file_fd, buffer, block_size, "SERVER: Read file content");
            
            /* 4. Read content from the file in his file system block size by block size */
            Send_Data(socket, buffer, block_size, "SERVER: WRITE file to socket");
            
            bytes_to_send -= block_size;
            Clear_Buffer(buffer, block_size+1);
        }
        /* close file in the server's file system */
        if(close(file_fd) == -1){
            Print_Error("SERVER: Close file");
        }

    }
    else{    /* for termination message */

        printf("[Worker Thread: %ld]  ----> SENDS < %s, %d > \n", pthread_self(), path_to_file, socket);
        /* 1. send to a client the length of the termination message */
        uint16_t file_length = 0;
        file_length = htons(strlen(path_to_file));       /* convert from host order to Network Byte Order */
        Send_Data(socket, &file_length, sizeof(uint16_t), "SERVER: WRITE term msg length");

        /* 2. send 0 size to a client */
        file_size = htonl(0);                            /* convert from host order to Network Byte Order */
        Send_Data(socket, &file_size, sizeof(uint32_t), "SERVER: WRITE term msg size");
        
        /* 3. send to a client the termination message */
        Send_Data(socket, (char*)path_to_file, strlen(path_to_file), "SERVER: WRITE term msg");
    
    }
    free(buffer);
}




/********************************************************************************************************************/

/* server creates as many worker thread as specified from the command line and initializes mutexes
   and condition variables needed */
void ThreadPool_Initialize(Worker_Threads_Args* args){

    int total_worker_threads = args->total_worker_threads;
    args->worker_threads = (pthread_t*)calloc(total_worker_threads, sizeof(pthread_t));
    int err = -1;
    pthread_mutex_init(&mutex_files_queue, NULL);
    pthread_mutex_init(&mutex_socket_queue, NULL);
    pthread_mutex_init(&mutex_dir, NULL);
    pthread_cond_init(&cond_queue_not_empty, NULL);
    pthread_cond_init(&cond_queue_not_full, NULL);

    for(int i=0; i < total_worker_threads; i++){
        if ((err = pthread_create(&args->worker_threads[i], NULL, ThreadPool_WorkerThread_Runs, (void*)args)) != 0) { /* New worker thread */
            Print_Error_Value("SERVER: [Worker Threads] Error in pthread_create", err);
        }
        // printf("ORIGINAL THREAD: [%ld] WORKER THREAD: [%ld]\n", 
        // pthread_self(), args->worker_threads[i]);
    }

    return;
}

/********************************************************************************************************************/

/* Job routine of the worker threads; after they are initialized they run below function constantly until server closes.
   A worker: 1. removes a file from the queue and send it to the right client (through the right socket), when there 
                are files in the queue
             2. waits if there are currently no other files in the queue */

void* ThreadPool_WorkerThread_Runs(void* arguments){

    Worker_Threads_Args args = *(Worker_Threads_Args*)arguments;
    size_t block_size = args.block_size;
    
    /* worker threads are running constantly until server receives ^C */
    while(RUNNING){   

        /* 1. try to remove a file name from the queue */
        pthread_mutex_lock(&mutex_files_queue);                         // lock queue mutex
        while(Queue_isEmpty(Files_Queue) && RUNNING){                   // wait while queue is empty 
            pthread_cond_wait(&cond_queue_not_empty, &mutex_files_queue);
        }
        if (!RUNNING){                                  /* needed when SIGINT is sent to server */
            pthread_mutex_unlock(&mutex_files_queue);   /* thread which has the mutex when ^C is sent, releases it */
            break;
        } 
        QNode popped_node = Queue_Pop(Files_Queue);                     // remove a file - socket from the queue 
        char* path_to_file = QueueNode_GetFileName(popped_node);        // get file name
        int socket = QueueNode_GetSocket(popped_node);                  // get the socket to send to the file name
        
        QNode mtx_sock_node = Queue_Find(Mutex_Socket_Queue, socket);   // find the socket - mutex node 
        QueueNode_LockMutex(mtx_sock_node);                             // lock the mutex for this specific socket so that 
                                                                        // no other worker can send files to the same client at the same time 
        
        pthread_mutex_unlock(&mutex_files_queue);                       // unlock queue mutex
        pthread_cond_signal(&cond_queue_not_full);                      // notify communication threads to continue inserting files 

        Server_Send_Files_to_Client(socket, path_to_file, block_size);  // send file to client 
        QueueNode_Delete(popped_node);                                  // delete mode from queue 

        QueueNode_UnlockMutex(mtx_sock_node);                           // unlock the mutex for this specific socket so that another
                                                                        // worker can send files to that client (aka socket)  

    }

    return NULL;

}


/********************************************************************************************************************/


/* Servers has received ^C and must destroy all worker threads along with the mutexes and conditional variables */
void ThreadPool_Destroy(Worker_Threads_Args* args){

    /* wakes up all working threads, which are waiting at the cond variable because the queue is empty */
    pthread_cond_broadcast(&cond_queue_not_empty);
    
    int total_worker_threads = args->total_worker_threads;
    for(int i=0; i < total_worker_threads; i++){
        pthread_join(args->worker_threads[i], NULL);
        printf("[Worker Thread: %ld] -----> Exits\n", 
        args->worker_threads[i]);
    }
    free(args->worker_threads);

    pthread_mutex_destroy(&mutex_files_queue);
    pthread_mutex_destroy(&mutex_socket_queue);
    pthread_mutex_destroy(&mutex_dir);
    pthread_cond_destroy(&cond_queue_not_empty);
    pthread_cond_destroy(&cond_queue_not_full);

    return;
}