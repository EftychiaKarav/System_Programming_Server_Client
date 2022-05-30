#include "../headers/Client.h"


int main(int argc, char* argv[]){

    if(argc != TOTAL_ARGS_CLIENT){
        Print_Error("Wrong number of arguments in the command line");
    }

    uint16_t port = -1;
    char* directory = NULL;
    int socket_number = -1;

    struct sockaddr_in server;
    memset(&server, 0, sizeof(struct sockaddr_in));
    struct sockaddr *serverptr=(struct sockaddr *)&server;
	struct hostent* server_entity = NULL;

    /* Check if the command line arguments are correct */
    
    for(int i=1; i < TOTAL_ARGS_CLIENT; i+=2){

        if(!strcmp(argv[i], "-i")){
	        /* IPV dot-number into  binary form (network byte order) */
            // if (inet_aton(argv[i+1], &server_address) == 0)
            //     Print_Error("IP address is not valid");
            if ((server_entity = gethostbyaddr((const char*)&server.sin_addr.s_addr, sizeof(server.sin_addr.s_addr), AF_INET)) == NULL){
                herror("Given IP-address could not be resolved\n");
                exit(EXIT_FAILURE);
            }
		    printf("IP-address:%s Resolved to: %s\n", argv[i+1],server_entity->h_name);
        }

        else if(!strcmp(argv[i], "-p")){
            if(atoi(argv[i+1]) <= 0)
                Print_Error("Port should be a positive number");
            port = htons(atoi(argv[i+1]));
        }

        else if(!strcmp(argv[i], "-d")){
            directory = argv[i+1];
        }

        else{
            Print_Error("Invalid argument type\n \
            The correct are: -i --> server_ip, -p --> server_port, -d --> directory (relative path)");
        }
    }

    /* create socket */
    if ((socket_number = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    	Print_Error("Client: Could not create socket");


    /* connect to server */
    server.sin_family = AF_INET;       /* Internet domain */
    //memcpy(&server.sin_addr, server_entity->h_addr, server_entity->h_length);
    server.sin_port = port;         /* Server port */
    /* Initiate connection */
    if (connect(socket_number, serverptr, sizeof(server)) < 0)
	   Print_Error("Client could not connect to Server");
    
    printf("Connecting to %d in port %d\n", server.sin_addr.s_addr, port);

    /* Client Process */
    Client(socket_number, directory);
    
    /* Close the socket */
    if(close(socket_number) == -1){
        perror("CLIENT: Close socket");
        exit(EXIT_FAILURE);
    }

    /* Close file descriptors for stdin, stdout, stderr */
    close(STDOUT_FILENO);
    close(STDIN_FILENO);
    close(STDERR_FILENO);

    exit(EXIT_SUCCESS);
}


/***************************************************************************************************************/



/* Process of the remoteClient -> the client is connected to the dataServer through a socket
   and asks for a directory from the server in order to copy it in its file system*/
void Client(int socket, char* directory){

    char* buffer = (char*)calloc(MAX_LENGTH, sizeof(char));
    uint32_t* numbers_buffer = (uint32_t*)calloc(1, sizeof(uint32_t)); 
    size_t block_size = 0;
    int num_bytes_read = -1;

    printf("CLIENT SOCKET %d\n", socket);
    if (write(socket, directory, strlen(directory)) < 0){
        perror("CLIENT: Write directory name");
        exit(EXIT_FAILURE);
    }
    
    if((num_bytes_read = read(socket, (char*)buffer, MAX_LENGTH)) < 0){
        perror("CLIENT: READ \"WRONG DIR NAME\" ");
        exit(EXIT_FAILURE);
    }
    int mess_length = strlen(CONFIRMATION_MSG) + strlen(directory) + strlen(DEFAULT_DIR);
    char* confirm_mess = (char*)calloc(mess_length + 1, sizeof(char));
    snprintf(confirm_mess, mess_length + 1, "%s%s%s%c", CONFIRMATION_MSG, DEFAULT_DIR, directory, '\0');
    //printf("%s\n", (char*)buffer);
    if(!strncmp((char*)buffer, WRONG_MSG, strlen(WRONG_MSG)))
        exit(EXIT_FAILURE);
    else if(!strncmp((char*)buffer, confirm_mess, strlen(confirm_mess))){
        
        printf("Waiting to get the files from server\n");
        printf("buffer: %s\n", (char*)buffer);
        if (write(socket, ACK_MSG, strlen(ACK_MSG)) < 0)
            perror("CLIENT: Write ACK message");

        if((num_bytes_read = read(socket, numbers_buffer, sizeof(uint32_t))) < 0){
            perror("CLIENT: READ block size ");
        }

        block_size = ntohl(*numbers_buffer);
        printf("WITHOUT: block size %d\t WITH: block size %ld\n",  *(uint32_t*)buffer, block_size);
        
        if (write(socket, ACK_MSG, strlen(ACK_MSG)) < 0)
            perror("CLIENT: Write ACK message");
    }
    free(confirm_mess);
    /*************************************************************************************************/
    printf("\n\nbefore start reading do -- while\n");

    Client_CopyFiles(socket, buffer, block_size);


    if (write(socket, TERM_MSG, strlen(TERM_MSG)) < 0){
        perror("CLIENT: Write ACK message");
        exit(EXIT_FAILURE);
    }


    free(buffer);
    free(numbers_buffer);
    return;
}


/***************************************************************************************************************/



/* Client 1. creates the [output_dir] which will contain the file system of the Server, which Client asked
          2. receives the files and copies them in the directory, after he creates the right hierarchy */        
void Client_CopyFiles(int socket, char* buffer, size_t block_size){

    /* 1. create [output_dir] --> "SERVER_COPY_[Client_pid]"  */
    char* output_dir = (char*)calloc(strlen(OUT_DIR) + 6, sizeof(char)); //5 is for pid and 1 for '\0'
    snprintf(output_dir, strlen(OUT_DIR) + 6, "%s%d%c", OUT_DIR, getpid(), '\0');
    
    struct stat dir_info;
    memset(&dir_info, 0, sizeof(struct stat));
    if (stat(output_dir, &dir_info) < 0){    // check if [output_dir] exists
        if(mkdir(output_dir, 0744) == -1){
            perror("CLIENT: Could not create output directory");
            exit(EXIT_FAILURE);  
        }
    }

    char* content_buffer = (char*)calloc(block_size+1, sizeof(char));   //buffer for reading file content
    
    do{
        Clear_Buffer(buffer, MAX_LENGTH);
        unsigned int file_size = 0;
        char* path = Client_Get_FileMetaData(socket, buffer, &file_size);
        printf("CLIENT GOT path: %s \n", path);

        int file_fd = -1;
        if(strcmp(path, TERMINATION_MSG) != 0){
            
            file_fd = Client_Resolve_FilePath(path, output_dir);
            if (write(socket, ACK_MSG, strlen(ACK_MSG)) < 0)
                perror("CLIENT: Write ACK message");


            printf("fd = %d wrote ack, before reading content\n", file_fd);
            /* GET THE CONTENT OF THE FILE*/

            int bytes_to_read = file_size;
            int bytes_read = 0;
            while(bytes_to_read){

                if(bytes_to_read < block_size)
                    block_size = bytes_to_read;
                if((bytes_read = read(socket, content_buffer, block_size)) < 0){
                    perror("CLIENT: Read file content from socket");
                }
                if(write(file_fd, content_buffer, block_size) < 0){
                    perror("SERVER: WRITE file content");
                    exit(EXIT_FAILURE);
                }
                Clear_Buffer(content_buffer, block_size);
                bytes_to_read -= block_size;
            }
            if(close(file_fd) == -1){      /* close file descriptor of the copied file */
                perror("CLIENT: Close file after copying");
                exit(EXIT_FAILURE);
            }

        }
        if (write(socket, ACK_MSG, strlen(ACK_MSG)) < 0)
            perror("CLIENT: Write ACK message");
        
        printf("Received: %s\n\n", path);
        free(path);
    }while(strncmp(buffer, TERMINATION_MSG, strlen(TERMINATION_MSG)) != 0);

    free(content_buffer);
    free(output_dir);

}

/******************************************************************************************************************************/



/* Read 1. the length of the path of the file
        2. the size of the file 
        3. the name of the file and return the name of the name of the file*/
char* Client_Get_FileMetaData(int socket, char* buffer, unsigned int* file_size){
    
    int num_bytes_read = -1;
    /* buffer to read the filepath_length and file_size */
    uint32_t* numbers_buffer = (uint32_t*)calloc(1, sizeof(uint32_t));
    memset(numbers_buffer, 0 , sizeof(uint32_t));

    /* 1. Receive the length of the path of the file */
    while((num_bytes_read = read(socket, numbers_buffer, sizeof(uint32_t))) < 0){
        perror("CLIENT: READ file path length ");
    }
    int file_length = ntohl(*numbers_buffer);     //convert from Network Byte Order to host order 
    printf("WITHOUT: file length %d\t WITH: file length %d\n",  *numbers_buffer, file_length);

    /* 2. Receive the size of the file */
    while((num_bytes_read = read(socket, numbers_buffer, sizeof(uint32_t))) < 0){
        perror("CLIENT: READ file size ");
    }
    *file_size = ntohl(*numbers_buffer);   //convert from Network Byte Order to host order
    printf("WITHOUT: file size %d\t WITH: file size %d\n",  *numbers_buffer, *file_size);

    /* 3. Receive the name of the path of the file along with the name of the file */
    while((num_bytes_read = read(socket, buffer, file_length)) < 0){
        perror("CLIENT: READ file name ");
        exit(EXIT_FAILURE);
    }

    char* path = (char*)calloc(file_length+1, sizeof(char));
    memcpy(path, buffer, strlen(buffer));

    free(numbers_buffer);

    return path;    /* return the path of the file */
}



/***************************************************************************************************************/

/* Client recreates the same file system hierarchy as the server does into a new directory with name:
   "SERVER_COPY_[pid_Client]", so that every client can have what they asked in a seperate directory;

   1. Given a path to a specific file, create the hierarchy of all the directories until we reach the file.
   2. Create the new file (it is deleted if it already exists)*/

int Client_Resolve_FilePath(char* path, char* output_dir){

    /* both start, end point at the begining of the path string */
    char* start = path;
    char* end = path;

    /* start building the file system hierarchy of the server at the client */
    /* The directory sent from the server will be copied inside [output_dir] */

    /* [copied_path] --> the substring of the whole path string currently copied into the [output_dir] */
    char* copied_path = (char*)calloc(strlen(path)+strlen(output_dir)+ 1, sizeof(char));
    memcpy(copied_path, output_dir, strlen(output_dir));
    
    
    struct stat node_info;   /* struct to check if a directory exists, while reading the path string*/
    short int isDirectory = 1;  /* given a path string all names are directories except for the last one; it is a file */
    int new_file_fd = -1;    /* file desc for the new file which will be created */

    
    while(strlen(start) != 0){  /* scan the whole path string until we reach the end of it */

        end = strstr(start, "/");  //search for '/' --> seperates the directories and files
        
        /* '/' not found --> there is no other directory; the last substring is a file */
        if(end == NULL){
            isDirectory = 0;
            end = start + strlen(start) - 1;   // points to the last character of the string
        }
        memset(&node_info, 0, sizeof(struct stat));

        /* copy the substring until the '/' to the [copied_path] */
        memcpy(copied_path + strlen(copied_path), start, strlen(start) - strlen(end) +1);
        
        int node_status = stat(copied_path, &node_info);
        
        /* create directory if it doesn't exist */
        if(isDirectory){
            if (node_status < 0){
                if(mkdir(copied_path, 0744) == -1){
                    perror("CLIENT: Could not create new directory");
                    exit(EXIT_FAILURE);  
                }
            }
        }
        else{          /* CREATE FILE -- DELETE IT IF EXISTS */
            if ( (new_file_fd = open(copied_path, O_CREAT|O_TRUNC|O_RDWR, 0644)) == -1){
                    perror("CLIENT: Creating new file");
                    exit(EXIT_FAILURE);
            }
        }       
        start = end + 1;   //[start] points to character after the '/'
    }
    
    free(copied_path);
    printf("new file fd %d\n", new_file_fd);
    return new_file_fd;    /* file descriptor of the new file */

}


/******************************************************************************************************************************/