#include "../headers/libraries.h"
#include "../headers/Queue.h"

pthread_mutex_t mutex_files_queue;
pthread_mutex_t mutex_socket_queue;
pthread_cond_t cond_queue_not_empty;
pthread_cond_t cond_queue_not_full;


void* Server_Job(void* arguments){

    Commun_Threads_Args args = *(Commun_Threads_Args*)arguments;
    int socket = args.socket;
    size_t block_size = args.block_size;
    int max_queue_size = args.queue_size;
    
    pthread_mutex_lock(&mutex_socket_queue);
    QNode q_node = QueueNode_Create_Node(socket, NULL);
    Queue_Insert(Mutex_Socket_Queue, q_node);
    pthread_mutex_unlock(&mutex_socket_queue);

    printf("socket server  is %d\n", socket);
    printf("blocksize %ld\n", block_size);
    int num_bytes_read = -1;
    char buffer[MAX_LENGTH] = {'\0'};
    printf("NEW THREAD:  %ld\n", pthread_self());
    int err;
    if ((err = pthread_detach(pthread_self())) != 0) {/* Detach thread */
       Print_Error_Value("Error in pthread_detach", err);
    }
    if((num_bytes_read = read(socket, buffer, MAX_LENGTH)) < 0){
        perror("SERVER: Read directory name");
    }
    /* build the path of the directory to open; it is relevant to the DEFAULT_DIR */
    char* path = (char*)calloc(strlen(DEFAULT_DIR) + strlen(buffer) + 1, sizeof(char));
    memcpy(path, DEFAULT_DIR, strlen(DEFAULT_DIR));
    memcpy(path + strlen(DEFAULT_DIR), buffer, strlen(buffer));
    printf("path is : %s\n", path);
    Clear_Buffer(buffer, MAX_LENGTH);
    DIR* dir_ptr = NULL;
    /* Open directory */
    if((dir_ptr = opendir(path)) == NULL ){
        fprintf(stdout, "Cannot open %s directory\n", path);
        Clear_Buffer(buffer, MAX_LENGTH);
        if(write(socket, WRONG_MSG, strlen(WRONG_MSG)) < 0){
            perror("SERVER: WRITE \"WRONG DIR NAME\" ");
        }
        printf("THREAD %ld is exiting\n", pthread_self());
        pthread_exit(NULL);    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!maybe change the value to indicate error
    }
    /* Close opened directory */
    if(closedir(dir_ptr) < 0 ){
        fprintf(stdout, "Cannot close %s directory\n", path);
        exit(EXIT_FAILURE);
    }

    char* mess = (char*)calloc(strlen(CONFIRMATION_MSG) + strlen(path) + 1, sizeof(char));
    snprintf(mess, strlen(CONFIRMATION_MSG) + strlen(path) + 1, "%s%s%c", CONFIRMATION_MSG, path, '\0');
    printf("total:   %s\n", mess);
    if(write(socket, mess, strlen(mess)) < 0){
        perror("SERVER: WRITE \"TOTAL FILES TO SEND:\" ");
    }

    while(strncmp(buffer, ACK_MSG, strlen(ACK_MSG)) != 0){
        printf("here\n");
        while((read(socket, buffer, strlen(ACK_MSG))) < 0){
            perror("SERVER: Read ACK message from client");
            exit(EXIT_FAILURE);
        }
        //Print_Error("SERVER: Could not receive ACK message from client");
    }
    uint32_t bl_size = htonl(block_size);
    if(write(socket, &bl_size, sizeof(uint32_t)) < 0){
        perror("SERVER: WRITE block size");
    }
    
    printf("buffer %s\n", buffer);


    while(strncmp(buffer, ACK_MSG, strlen(ACK_MSG)) != 0){
        printf("here\n");
        while((read(socket, buffer, strlen(ACK_MSG))) < 0){
            perror("SERVER: Read ACK message from client");
            exit(EXIT_FAILURE);
        }
        //Print_Error("SERVER: Could not receive ACK message from client");
    }


    printf("before extracting files\n");

    Extract_Files_From_Directory(socket, path, max_queue_size);

    printf("[%ld] FINISHED WITH EXTRACTING FILES\n", pthread_self());
    QNode node = QueueNode_Create_Node(socket, TERMINATION_MSG);
        
    pthread_mutex_lock(&mutex_files_queue);
    while(Queue_Size(Files_Queue) >= max_queue_size){
        pthread_cond_wait(&cond_queue_not_full, &mutex_files_queue);
    }
    Queue_Insert(Files_Queue, node);
    pthread_mutex_unlock(&mutex_files_queue);

    pthread_cond_signal(&cond_queue_not_empty);   //notify that workers can start


    //Queue_Print(Files_Queue);
    printf("\n\n*****START SENDING FILES*****\n");
    int FINISHED = 0;
    while(!FINISHED){
        
        if ((read(socket, buffer, strlen(ACK_MSG))) < 0){
            perror("SERVER: Read ACK message from client");
            exit(EXIT_FAILURE);
        }
        if (strncmp(buffer, TERM_MSG, strlen(TERM_MSG)) == 0){
            printf("FIINISHED\n");
            FINISHED = 1;
        }            
        else if(strncmp(buffer, ACK_MSG, strlen(ACK_MSG)) == 0){
            printf("buffer %s [%ld]\n", buffer, pthread_self());    
            Clear_Buffer(buffer, MAX_LENGTH);
        }

    }

    
    //Send_Files_to_Client(block_size);



    pthread_mutex_lock(&mutex_socket_queue);
    Queue_Delete(Mutex_Socket_Queue, q_node);
    pthread_mutex_unlock(&mutex_socket_queue);

    //sleep(10);
    printf("THREAD %ld is exiting\n", pthread_self());
    pthread_exit(NULL);

}



/******************************************************************************************************************/

void Extract_Files_From_Directory(int socket, char* path, int max_queue_size){

    /* Save the type of the node; directory or regular file */
    struct stat node;
    memset(&node, 0, sizeof(struct stat));

    /* get node status */
    if(stat(path, &node) < 0 ){
        perror("SERVER: Stat while reading directory");
        exit(EXIT_FAILURE);
    }

    /* test against the S_IFMT flag to get the first 4 bits of the node to determine the file type */
    mode_t outcome = node.st_mode & S_IFMT;   
    if (outcome == S_IFDIR){    /* directory */
        
        DIR* dir_ptr = NULL;    // pointer to an opened directory
        struct 	dirent *dir_entity = NULL;   //struct for items in the directory

        /* Open directory */
        if((dir_ptr = opendir(path)) == NULL ){
            fprintf(stdout, "Cannot open %s directory\n", path);
            exit(EXIT_FAILURE);
        }

        /* keep the current path */
        char* curr_path = (char*)calloc(strlen(path)+ 1 + 1, sizeof(char));   //1 for '/' one for '\0'
        memcpy(curr_path, path, strlen(path));
        memcpy(curr_path + strlen(path), "/", 1);
        //printf("Directory name: %s\n", curr_path);

        /* Read directory */
        while ( (dir_entity = readdir(dir_ptr)) != NULL ){
            
            /* if the directory is not cwd or pwd */
            if((strcmp(dir_entity->d_name, ".") != 0) && (strcmp(dir_entity->d_name, "..") != 0)){

                /* add the new item to the path */
                char* final_path = (char*)calloc(strlen(curr_path)+ strlen(dir_entity->d_name) + 1, sizeof(char));
                memcpy(final_path, curr_path, strlen(curr_path));
                memcpy(final_path + strlen(curr_path), dir_entity->d_name, strlen(dir_entity->d_name));
                
                /* Recursion: input is the new path which is the path to the new item;
                the function is called again with the new path and determines if the item is directory or regular file*/
                Extract_Files_From_Directory(socket, final_path, max_queue_size);

                free(final_path);
            }

        }

        /* delete current path */
        //memset(curr_path, 0, strlen(curr_path));
        free(curr_path);

        /* Close directory */
        if(closedir(dir_ptr) < 0 ){
            fprintf(stdout, "Cannot close %s directory\n", path);
            exit(EXIT_FAILURE);
        }
        
    }
    else if(outcome == S_IFREG){    //regular file 
        QNode node = QueueNode_Create_Node(socket, path);
        
        pthread_mutex_lock(&mutex_files_queue);
        while(Queue_Size(Files_Queue) >= max_queue_size){
            pthread_cond_wait(&cond_queue_not_full, &mutex_files_queue);
        }
        printf("BEFORE INSERTING TO QUEUE: FILE name: %s\n", path);
        Queue_Insert(Files_Queue, node);
        pthread_mutex_unlock(&mutex_files_queue);

        pthread_cond_signal(&cond_queue_not_empty);   //notify that workers can start
    }
    /* return if item is a regular file */
    return;
}

/*********************************************************************************************************************/


void Send_Files_to_Client(size_t block_size){   //func for threads + mutexes

    pthread_mutex_lock(&mutex_files_queue);
    printf("SEND: thread [%ld] gets mutex -- blocksize %ld\n", pthread_self(), block_size);
    while(Queue_isEmpty(Files_Queue)){
        pthread_cond_wait(&cond_queue_not_empty, &mutex_files_queue);
    }
    QNode popped_node = Queue_Pop(Files_Queue);
    char* path_to_file = QueueNode_GetFileName(popped_node);
    int socket = QueueNode_GetSocket(popped_node);


    QNode mtx_sock_node = Queue_Find(Mutex_Socket_Queue, socket);
    pthread_mutex_t socket_mutex = QueueNode_GetMutex(mtx_sock_node);
    //printf("node -> %p, %d", (void*)mtx_sock_node, socket_mutex.__data.__lock);
    int err;
    printf("mutex lock is %d\n", socket_mutex.__data.__lock);
    QueueNode_LockMutex(mtx_sock_node);
    // if(pthread_mutex_lock(&socket_mutex) !=0){
    //     printf("error in lock\n");
    //     exit(EXIT_FAILURE);
    // }
    // if((err= pthread_mutex_trylock(&socket_mutex)) !=0){
    //     perror("error in trylock ");
    //     printf("%d\n", err);
    //     exit(EXIT_FAILURE);
    // }
    printf("^^^^^^^^ LOCK MUTEX FOR SOCKET ^^^^^^^^^   ---> [%ld]\n", pthread_self());

    //printf("node -> %p, %d\n", (void*)mtx_sock_node, socket_mutex.__data.__lock);
    pthread_mutex_unlock(&mutex_files_queue);
    pthread_cond_signal(&cond_queue_not_full);   //notify that workers can start


    uint32_t file_length = -1;
    uint32_t file_size = -1;
    struct stat file_info;
    char* buffer = (char*)calloc(block_size, sizeof(char));           //freeeeeeeeeeeeee
    printf("[%ld] SENDS ----> %s\n", pthread_self(),path_to_file);
    if(strcmp(path_to_file, TERMINATION_MSG) != 0){
        memset(&file_info, '\0', sizeof(file_info));
        /* find the size of the file */
        if(stat(path_to_file, &file_info) < 0){
            perror("SERVER: Could not get info for the file to process");
            exit(EXIT_FAILURE);
        }
        printf("FILE IS: %s\n", path_to_file);

        file_length = htonl(strlen(path_to_file));
        printf("WITHOUT: file length %ld\t WITH: file length %d\n",  strlen(path_to_file), file_length);
        if(write(socket, &file_length, sizeof(uint32_t)) < 0){
            perror("SERVER: WRITE file length");
        }

        file_size = htonl(file_info.st_size);
        printf("WITHOUT: file size %ld\t WITH: file size %d\n\n",  file_info.st_size, file_size);
        if(write(socket, &file_size, sizeof(uint32_t)) < 0){
            perror("SERVER: WRITE file size");
        }
        if(write(socket, path_to_file, strlen(path_to_file)) < 0){
            perror("SERVER: WRITE file name");
        }



        // while(strcmp(buffer, ACK_MSG) != 0){
        //     printf("inn\n");
        //     if ((read(socket, buffer, strlen(ACK_MSG))) < 0){
        //         perror("SERVER: Read ACK message from client");
        //         exit(EXIT_FAILURE);
        //     }   
        // }

        printf("[%ld]  ----> BEFORE OPENING FILE %s\n", pthread_self(), path_to_file);
        /* SENDING THE FILE*/
        int file_fd = -1;
        while ((file_fd = open(path_to_file, O_RDONLY)) < 0){
            //if (errno == EINTR) continue;
            perror("SERVER: Could not open file to read");
            exit(EXIT_FAILURE);	
        }
        int bytes_to_write = file_info.st_size, bytes_read = 0;
        printf("[%ld]  ----> BEFORE READING FILE %s\n", pthread_self(), path_to_file);
        while(bytes_to_write){

            if(bytes_to_write < block_size)
                block_size = bytes_to_write;
            //while(actual_bytes_read < block_size){
                if((bytes_read = read(file_fd, buffer, block_size)) < 0){
                    perror("SERVER: Read file content");
                }
                //actual_bytes_read += bytes_read;
            //}
            if(write(socket, buffer, block_size) < 0){
                perror("SERVER: WRITE file to socket");
                exit(EXIT_FAILURE);
            }
            Clear_Buffer(buffer, block_size);
            bytes_to_write -= block_size;
        }
        printf("SERVER: after sending file to socket\n");


    }
    else{
        file_length = htonl(strlen(path_to_file));
        printf("WITHOUT: file length %ld\t WITH: file length %d\n",  strlen(path_to_file), file_length);
        if(write(socket, &file_length, sizeof(uint32_t)) < 0){
            perror("SERVER: WRITE file length");
        }

        file_size = htonl(0);
        printf("WITHOUT: file size %d\t WITH: file size %d\n\n",  file_size, file_size);
        if(write(socket, &file_size, sizeof(uint32_t)) < 0){
            perror("SERVER: WRITE file size");
        }
        if(write(socket, path_to_file, strlen(path_to_file)) < 0){
            perror("SERVER: WRITE file name");
        }

    }


    // while((read(socket, buffer, strlen(ACK_MSG))) < 0){
    //     perror("SERVER: Read ACK message from client");
    // }
    // printf("%s\n", buffer);
    // if(strcmp(buffer, ACK_MSG) != 0){
    //     Print_Error("SERVER: Could not receive ACK message from client");
    // }
    // printf("received ack after finished with one file\n");
    QueueNode_Delete(popped_node);
    //}

    free(buffer);
    printf("^^^^^^^^ UNLOCK MUTEX FOR SOCKET ^^^^^^^^^   ---> [%ld]\n", pthread_self());
    // if (pthread_mutex_unlock(&socket_mutex))
    //     printf("error in unlock");
    QueueNode_UnlockMutex(mtx_sock_node);

}





/********************************************************************************************************************/


int main(int argc, char* argv[]){


    if(argc != TOTAL_ARGS_SERVER){
        Print_Error("Wrong number of arguments in the command line");
    }

    int thread_pool_size = -1, queue_size = -1, socket_number = -1, new_socket_number = -1;
    uint16_t port = -1;
    size_t block_size = -1;

    for(int i=1; i < TOTAL_ARGS_SERVER; i+=2){
        if(!strcmp(argv[i], "-p")){
            if(atoi(argv[i+1]) <= 0)
                Print_Error("Port should be a positive number");
            port = htons(atoi(argv[i+1]));
        }

        else if(!strcmp(argv[i], "-s")){
            
            if(atoi(argv[i+1]) <= 0)
                Print_Error("Thread Pool size should be a positive number");
            thread_pool_size = atoi(argv[i+1]);
        }

        else if(!strcmp(argv[i], "-q")){
            if(atoi(argv[i+1]) <= 0)
                Print_Error("Queue Size should be a positive number");
            queue_size = atoi(argv[i+1]);
        }

        else if(!strcmp(argv[i], "-b")){
            if(atoi(argv[i+1]) <= 0)
                Print_Error("Block Size should be a positive number");
            block_size = atoi(argv[i+1]);
        }

        else{
            Print_Error("Invalid argument type\n \
            The correct are: -p --> server_port, -s --> thread_pool_size \
            -q --> queue_size, -b --> block_size");
        }
    }

    /* Ignore all signals at this point */
    struct sigaction server_action;
    memset(&server_action, 0, sizeof(struct sigaction));
    server_action.sa_handler = Stop_Server;
	sigaction(SIGINT, &server_action, NULL);
    server_action.sa_handler = SIG_DFL;
    sigaction(SIGTSTP, &server_action, NULL);


    struct sockaddr_in server, client;
    socklen_t clientlen = 1;
    struct sockaddr *serverptr=(struct sockaddr *)&server;
    struct sockaddr *clientptr=(struct sockaddr *)&client;
    struct hostent* client_entity = NULL;

    memset(&server, 0, sizeof(struct sockaddr_in));
    memset(&client, 0, sizeof(struct sockaddr_in));



    /* Create socket */
    if ((socket_number = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        Print_Error("Server: Could not create socket");
    
    server.sin_family = AF_INET;       /* Internet domain */
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = port;      /* The given port */
    
    int value = 1;
    
    if (setsockopt(socket_number, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(int)) < 0)
        Print_Error("Error in sockopt");
    printf("SERVER's IP is: %d\n", server.sin_addr.s_addr);
    /* Bind socket to address */
    if (bind(socket_number, serverptr, sizeof(server)) < 0)
        Print_Error("Server: Could not bind");
    
    /* Listen for connections */
    if (listen(socket_number, 20) < 0)
        Print_Error("Server: listen failed");
    
    printf("Listening for connections to port %d\n", port);

    pthread_mutex_init(&mutex_files_queue, NULL);
    pthread_mutex_init(&mutex_socket_queue, NULL);
    pthread_cond_init(&cond_queue_not_empty, NULL);
    pthread_cond_init(&cond_queue_not_full, NULL);

    Worker_Threads_Args* args;
    //memset(args, 0, sizeof(Worker_Threads_Args));
    args = (Worker_Threads_Args*)calloc(1, sizeof(Worker_Threads_Args));
    args->block_size = block_size;
    args->queue_size = queue_size;
    args->total_worker_threads = thread_pool_size;


    if(!Queue_Exists(Files_Queue)){
        Files_Queue = Queue_Initialize();
    }
    
    ThreadPool_Initialize(args);

    while(RUNNING){

        /* accept connection */
    	if ((new_socket_number = accept(socket_number, clientptr, &clientlen)) < 0){
            if(errno == EINTR) continue;
            perror("Server could not accept connection");
            printf("errno is %d\n", errno);
            exit(EXIT_FAILURE);
        }
    	/* Find client's address */
    	if ((client_entity = gethostbyaddr((char *) &client.sin_addr.s_addr, sizeof(client.sin_addr.s_addr), client.sin_family)) == NULL){
            herror("Server could not resolve client's IP address");
            exit(EXIT_FAILURE);
        }
    	
        printf("Accepted connection from %s\n", client_entity->h_name);
    	printf("Accepted connection\n");
        printf("new socket is %d\n", socket_number);
        printf("new socket is %d\n", new_socket_number);

        if(!Queue_Exists(Mutex_Socket_Queue)){
            Mutex_Socket_Queue = Queue_Initialize();
        }

        pthread_t communication_thread;
        Commun_Threads_Args args;
        memset(&args, 0, sizeof(Commun_Threads_Args));
        args.socket = new_socket_number;
        args.block_size = block_size;
        args.queue_size = queue_size;
        int err = 0;
        if ((err = pthread_create(&communication_thread, NULL, Server_Job, (void*)(&args))) != 0) { /* New thread */
            Print_Error_Value("Error in pthread_create", err);
        }
        printf("ORIGINAL THREAD: [%ld] COMMUNICATION THREAD: [%ld]\n", 
            pthread_self(), communication_thread);


    	// close(new_socket_number); /* parent closes socket to client            */
		// 	/* must be closed before it gets re-assigned */

    }
    Queue_Destroy(Files_Queue);
    Queue_Destroy(Mutex_Socket_Queue);
    ThreadPool_Destroy(args);
    pthread_mutex_destroy(&mutex_files_queue);
    pthread_mutex_destroy(&mutex_socket_queue);
    pthread_cond_destroy(&cond_queue_not_empty);
    pthread_cond_destroy(&cond_queue_not_full);


    exit(EXIT_SUCCESS);
}
/********************************************************************************************************************/



void* ThreadPool_WorkerThread_Runs(void* arguments){

    Worker_Threads_Args args = *(Worker_Threads_Args*)arguments;
    size_t block_size = args.block_size;
    printf("block size %ld\n", block_size);
    while(RUNNING){
        printf("[Thread %ld] takes another task\n", pthread_self());
        Send_Files_to_Client(block_size);

    }

    return NULL;

}


void ThreadPool_Initialize(Worker_Threads_Args* args){

    int total_worker_threads = args->total_worker_threads;
    args->worker_threads = (pthread_t*)calloc(total_worker_threads, sizeof(pthread_t));
    int err = -1;

    for(int i=0; i < total_worker_threads; i++){
        if ((err = pthread_create(&args->worker_threads[i], NULL, ThreadPool_WorkerThread_Runs, (void*)args)) != 0) { /* New worker thread */
            Print_Error_Value("SERVER: [WORKER THREADS] Error in pthread_create", err);
        }
        printf("ORIGINAL THREAD: [%ld] WORKER THREAD: [%ld]\n", 
        pthread_self(), args->worker_threads[i]);
    }
    
    return;
}


void ThreadPool_Destroy(Worker_Threads_Args* args){

    int total_worker_threads = args->total_worker_threads;

    pthread_cond_broadcast(&cond_queue_not_empty);
    for(int i=0; i < total_worker_threads; i++){
        pthread_join(args->worker_threads[i], NULL);
        printf("WORKER THREAD: [%ld] -----> EXITING\n", 
        args->worker_threads[i]);
    }
    free(args->worker_threads);
    return;
}