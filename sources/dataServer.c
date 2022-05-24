#include "../headers/libraries.h"


void* Server_Job(void* arg){

    int socket = *(int*)arg;
    int num_bytes_read = -1;
    char buffer[MAX_LENGTH] = '\0';
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

    DIR* dir_ptr = NULL;
    /* Open directory */
    if((dir_ptr = opendir(path)) == NULL ){
        fprintf(stdout, "Cannot open %s directory\n", path);
        memset(buffer, 0, MAX_LENGTH);
        if(write(socket, WRONG_MSG, strlen(WRONG_MSG)) < 0){
            perror("SERVER: WRITE \"WRONG DIR NAME\" ");
        }
        printf("THREAD %ld is exiting\n", pthread_self());
        pthread_exit(NULL);    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!maybe change the value to indicate error
    }
    
    if(closedir(dir_ptr) < 0 ){
        fprintf(stdout, "Cannot close %s directory\n", path);
        exit(EXIT_FAILURE);
    }


    sleep(10);
    printf("THREAD %ld is exiting\n", pthread_self());
    pthread_exit(NULL);

}



char* Extract_Files_From_Directory(char* path){

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
        printf("Directory name: %s\n", curr_path);

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
                final_path = Extract_Files_From_Directory(final_path);

                free(final_path);
            }

        }

        /* delete current path */
        memset(curr_path, 0, strlen(curr_path));
        free(curr_path);

        /* Close directory */
        if(closedir(dir_ptr) < 0 ){
            fprintf(stdout, "Cannot close %s directory\n", path);
            exit(EXIT_FAILURE);
        }
        
    }
    else if(outcome == S_IFREG){    //regular file 
        printf("FILE name: %s\n", path);
    }
    /* return if item is a regular file */
    return path;
}


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
    memset(&server, 0, sizeof(struct sockaddr_in));
    memset(&client, 0, sizeof(struct sockaddr_in));

    socklen_t clientlen;
    struct sockaddr *serverptr=(struct sockaddr *)&server;
    struct sockaddr *clientptr=(struct sockaddr *)&client;
    struct hostent* client_entity = NULL;

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

    int pid = -1;
    while(RUNNING){

        /* accept connection */
    	if ((new_socket_number = accept(socket_number, clientptr, &clientlen)) < 0)
            Print_Error("Server could not accept connection");
    	/* Find client's address */
    	if ((client_entity = gethostbyaddr((char *) &client.sin_addr.s_addr, sizeof(client.sin_addr.s_addr), client.sin_family)) == NULL){
            herror("Server could not resolve client's IP address");
            exit(EXIT_FAILURE);
        }
    	
        printf("Accepted connection from %s\n", client_entity->h_name);
    	printf("Accepted connection\n");


        pthread_t communication_thread;
        int err, status;
        if ((err = pthread_create(&communication_thread, NULL, Server_Job, (void*)(&new_socket_number))) != 0) { /* New thread */
            Print_Error_Value("Error in pthread_create", err);
        }
        printf("I am original thread %ld and I created thread %ld\n", 
                pthread_self(), communication_thread);


    	close(new_socket_number); /* parent closes socket to client            */
			/* must be closed before it gets re-assigned */

    }


    exit(EXIT_SUCCESS);
}
