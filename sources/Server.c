#include "../headers/Server.h"

pthread_mutex_t mutex_files_queue;
pthread_mutex_t mutex_socket_queue;
pthread_cond_t cond_queue_not_empty;
pthread_cond_t cond_queue_not_full;



void* Server(void* arguments){

    Commun_Threads_Args args = *(Commun_Threads_Args*)arguments;
    int socket = args.socket;
    size_t block_size = args.block_size;
    int max_queue_size = args.queue_size;
    
    pthread_mutex_lock(&mutex_socket_queue);
    QNode q_node = QueueNode_Create_Node(socket, NULL);
    Queue_Insert(Mutex_Socket_Queue, q_node);
    pthread_mutex_unlock(&mutex_socket_queue);

    printf("socket server  is %d\n", socket);
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
    free(mess);
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

    Server_Extract_Files_From_Directory(socket, path, max_queue_size);
    free(path);
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
            printf("FIINISHED [%ld]\n", pthread_self());
            FINISHED = 1;
        }            
        else if(strncmp(buffer, ACK_MSG, strlen(ACK_MSG)) == 0){
            //printf("buffer %s [%ld]\n", buffer, pthread_self());    
            Clear_Buffer(buffer, MAX_LENGTH);
        }

    }

    
    //Send_Files_to_Client(block_size);



    pthread_mutex_lock(&mutex_socket_queue);
    Queue_Delete(Mutex_Socket_Queue, q_node);
    pthread_mutex_unlock(&mutex_socket_queue);
    if(close(socket) == -1){
        perror("SERVER: Close new socket");
        exit(EXIT_FAILURE);
    }
    //sleep(10);
    printf("THREAD %ld is exiting\n", pthread_self());
    pthread_exit(NULL);

}



/******************************************************************************************************************/

void Server_Extract_Files_From_Directory(int socket, char* path, int max_queue_size){

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
                Server_Extract_Files_From_Directory(socket, final_path, max_queue_size);

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


void Send_Files_to_Client(int socket, const char* path_to_file, size_t block_size){   //func for threads + mutexes

    uint32_t file_length = -1;
    uint32_t file_size = -1;
    int file_fd = -1;
    struct stat file_info;
    char* buffer = (char*)calloc(block_size + 1, sizeof(char));           //freeeeeeeeeeeeee
    printf("[%ld] SENDS ----> %s\n", pthread_self(),path_to_file);
    if(strcmp(path_to_file, TERMINATION_MSG) != 0){
        memset(&file_info, '\0', sizeof(file_info));
        /* find the size of the file */
        if(stat(path_to_file, &file_info) < 0){
            perror("SERVER: Could not get info for the file to process");
            exit(EXIT_FAILURE);
        }
        printf("FILE IS: %s\n", path_to_file);

        file_length = htons(strlen(path_to_file));
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

        printf("[%ld]  ----> BEFORE OPENING FILE %s\n", pthread_self(), path_to_file);
        /* SENDING THE FILE*/
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

        if(close(file_fd) == -1){
            perror("SERVER: Close file");
            exit(EXIT_FAILURE);
        }

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
    free(buffer);
}





/********************************************************************************************************************/





void* ThreadPool_WorkerThread_Runs(void* arguments){

    Worker_Threads_Args args = *(Worker_Threads_Args*)arguments;
    size_t block_size = args.block_size;
    printf("block size %ld\n", block_size);
    while(RUNNING){
        printf("[Thread %ld] takes another task\n", pthread_self());



        pthread_mutex_lock(&mutex_files_queue);
        printf("SEND: thread [%ld] gets mutex -- blocksize %ld\n", pthread_self(), block_size);
        while(Queue_isEmpty(Files_Queue) && RUNNING){
            pthread_cond_wait(&cond_queue_not_empty, &mutex_files_queue);
        }
        if (!RUNNING){
            //printf("before unlocking\n");
            pthread_mutex_unlock(&mutex_files_queue);
            break;
        } 
        QNode popped_node = Queue_Pop(Files_Queue);
        char* path_to_file = QueueNode_GetFileName(popped_node);
        int socket = QueueNode_GetSocket(popped_node);
        QNode mtx_sock_node = Queue_Find(Mutex_Socket_Queue, socket);
        QueueNode_LockMutex(mtx_sock_node);
        printf("^^^^^^^^ LOCK MUTEX FOR SOCKET ^^^^^^^^^   ---> [%ld]\n", pthread_self());
        pthread_mutex_unlock(&mutex_files_queue);
        pthread_cond_signal(&cond_queue_not_full);   //notify that workers can start

        Send_Files_to_Client(socket, path_to_file, block_size);
        QueueNode_Delete(popped_node);

        printf("^^^^^^^^ UNLOCK MUTEX FOR SOCKET ^^^^^^^^^   ---> [%ld]\n", pthread_self());
        // if (pthread_mutex_unlock(&socket_mutex))
        //     printf("error in unlock");
        QueueNode_UnlockMutex(mtx_sock_node);

    }
    //printf("returning null\n");

    return NULL;

}


void ThreadPool_Initialize(Worker_Threads_Args* args){

    int total_worker_threads = args->total_worker_threads;
    args->worker_threads = (pthread_t*)calloc(total_worker_threads, sizeof(pthread_t));
    int err = -1;
    pthread_mutex_init(&mutex_files_queue, NULL);
    pthread_mutex_init(&mutex_socket_queue, NULL);
    pthread_cond_init(&cond_queue_not_empty, NULL);
    pthread_cond_init(&cond_queue_not_full, NULL);

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
    //pthread_mutex_lock(&mutex_files_queue);
    pthread_cond_broadcast(&cond_queue_not_empty);
    //pthread_mutex_unlock(&mutex_files_queue);
    //printf("signaled broadcast\n");
    for(int i=0; i < total_worker_threads; i++){
        pthread_join(args->worker_threads[i], NULL);
        printf("WORKER THREAD: [%ld] -----> EXITING\n", 
        args->worker_threads[i]);
    }
    free(args->worker_threads);

    pthread_mutex_destroy(&mutex_files_queue);
    pthread_mutex_destroy(&mutex_socket_queue);
    pthread_cond_destroy(&cond_queue_not_empty);
    pthread_cond_destroy(&cond_queue_not_full);

    return;
}