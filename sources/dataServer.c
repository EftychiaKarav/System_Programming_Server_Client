#include "../headers/libraries.h"
#include "../headers/Queue.h"


void* Server_Job(void* arguments){

    Commun_Threads_Args args = *(Commun_Threads_Args*)arguments;
    int socket = args.socket;
    size_t block_size = args.block_size;
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

    Extract_Files_From_Directory(socket, path);
    QNode node = QueueNode_Create_Node(socket, TERMINATION_MSG);
    Queue_Insert(Files_Queue, node);
    //Queue_Print(Files_Queue);
    char* mess = (char*)calloc(26, sizeof(char));
    snprintf(mess, strlen(FILES_SENT_MSG) + 5, "%s%d", FILES_SENT_MSG, Queue_Size(Files_Queue));
    printf("total:   %s\n", mess);
    if(write(socket, mess, strlen(mess)) < 0){
        perror("SERVER: WRITE \"TOTAL FILES TO SEND:\" ");
    }
    Clear_Buffer(buffer, MAX_LENGTH);
    uint32_t bl_size = htonl(block_size);
    if(write(socket, &bl_size, sizeof(uint32_t)) < 0){
        perror("SERVER: WRITE block size");
    }
    
    printf("buffer %s\n", buffer);
    while(strcmp(buffer, ACK_MSG) != 0){
        while((read(socket, buffer, strlen(ACK_MSG))) < 0){
            perror("SERVER: Read ACK message from client");
            exit(EXIT_FAILURE);
        }
        //Print_Error("SERVER: Could not receive ACK message from client");
    }
    printf("before calling send files func\n");
    Send_Files_to_Client(block_size);






    sleep(10);
    printf("THREAD %ld is exiting\n", pthread_self());
    pthread_exit(NULL);

}



/******************************************************************************************************************/

void Extract_Files_From_Directory(int socket, char* path){

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
                Extract_Files_From_Directory(socket, final_path);

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
        //printf("FILE name: %s\n", path);
        QNode node = QueueNode_Create_Node(socket, path);
        Queue_Insert(Files_Queue, node);
    }
    /* return if item is a regular file */
    return;
}

/*********************************************************************************************************************/


void Send_Files_to_Client(size_t block_size){   //func for threads + mutexes

    struct stat file_info;
    char* buffer = (char*)calloc(block_size, sizeof(char));           //freeeeeeeeeeeeee
    while(Queue_Size(Files_Queue) != 0){

        QNode popped_node = Queue_Pop(Files_Queue);
        char* path_to_file = QueueNode_GetFileName(popped_node);
        int socket = QueueNode_GetSocket(popped_node);
        uint32_t file_length = -1;
        uint32_t file_size = -1;

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



            while(strcmp(buffer, ACK_MSG) != 0){
                printf("inn\n");
                if ((read(socket, buffer, strlen(ACK_MSG))) < 0){
                    perror("SERVER: Read ACK message from client");
                    exit(EXIT_FAILURE);
                }   
            }

            printf("ok before opeming file\n");
            /* SENDING THE FILE*/
            int file_fd = -1;
            while ((file_fd = open(path_to_file, O_RDONLY)) < 0){
                if (errno == EINTR) continue;
                perror("SERVER: Could not open file to read");
                exit(EXIT_FAILURE);	
            }
            int bytes_to_write = file_info.st_size, bytes_read = 0;
            printf("SERVER: before reading from file_fd\n");
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


        while((read(socket, buffer, strlen(ACK_MSG))) < 0){
            perror("SERVER: Read ACK message from client");
        }
        printf("%s\n", buffer);
        if(strcmp(buffer, ACK_MSG) != 0){
            Print_Error("SERVER: Could not receive ACK message from client");
        }
        printf("received ack after finished with one file\n");
        QueueNode_Delete(popped_node);
    }

    free(buffer);

}





/********************************************************************************************************************/


int main(int argc, char* argv[]){


    if(argc != TOTAL_ARGS_SERVER){
        Print_Error("Wrong number of arguments in the command line");
    }

    int thread_pool_size = -1, queue_size = -1, socket_number = -1, new_socket_number = -1;
    uint16_t port = -1;
    size_t block_size = -1;
    int RUNNING = 1;

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

    if(!Queue_Exists(Files_Queue)){
        Files_Queue = Queue_Initialize();
    }
    while(RUNNING){

        /* accept connection */
    	if ((new_socket_number = accept(socket_number, clientptr, &clientlen)) < 0){
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


        pthread_t communication_thread;
        Commun_Threads_Args args;
        memset(&args, 0, sizeof(Commun_Threads_Args));
        args.socket = new_socket_number;
        args.block_size = block_size;
        int err;
        if ((err = pthread_create(&communication_thread, NULL, Server_Job, (void*)(&args))) != 0) { /* New thread */
            Print_Error_Value("Error in pthread_create", err);
        }
        printf("I am original thread %ld and I created thread %ld\n", 
                pthread_self(), communication_thread);


    	// close(new_socket_number); /* parent closes socket to client            */
		// 	/* must be closed before it gets re-assigned */

    }


    exit(EXIT_SUCCESS);
}
