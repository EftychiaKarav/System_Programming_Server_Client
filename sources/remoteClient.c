#include "../headers/libraries.h"
#include "../headers/Queue.h"


int Resolve_FilePath(char* path, char* default_dir){

    
    char* start = path;
    char* end = path;
    char* temp_path = (char*)calloc(strlen(path)+strlen(default_dir)+ 1, sizeof(char));
    memcpy(temp_path, default_dir, strlen(default_dir));
    struct stat node_info;
    int is_Directory = 1;
    int new_file_fd = -1;

    while(strlen(start) != 0){

        end = strstr(start, "/");
        if(end == NULL){
            is_Directory = 0;
            end = start + strlen(start) - 1;
        }
        memset(&node_info, 0, sizeof(struct stat));
        memcpy(temp_path+strlen(temp_path), start, strlen(start) - strlen(end)+1);
        if (stat(temp_path, &node_info) < 0){

            if(is_Directory){
                if(mkdir(temp_path, 0744) == -1){
                    perror("CLIENT: Could not create new directory");
                    exit(EXIT_FAILURE);  
                }
            }
            else{
                if ( (new_file_fd = open(temp_path, O_CREAT|O_RDWR, 0644)) == -1){
                    perror("CLIENT: Creating new file");
                    exit(EXIT_FAILURE);
                }

            }
        }
        else{
            mode_t type = node_info.st_mode & S_IFMT;
            if(type == S_IFREG){
                if(unlink(temp_path) < 0){
                    perror("CLIENT: Could not remove file");
                    exit(EXIT_FAILURE);
                }
                if ( (new_file_fd = open(temp_path, O_CREAT|O_RDWR, 0644)) == -1){
                    perror("CLIENT: (after unlink)Creating new file");
                    exit(EXIT_FAILURE);
                }
            }
        }

        start = end + 1;
    }
    
    free(temp_path);
    return new_file_fd;

}


/******************************************************************************************************************************/



int main(int argc, char* argv[]){


    if(argc != TOTAL_ARGS_CLIENT){
        Print_Error("Wrong number of arguments in the command line");
    }

    uint16_t port = -1;
    char* directory = NULL;
    //char directory[MAX_PATH_LENGTH] = {'\0'};
    int socket_number = -1;

    struct sockaddr_in server;
    memset(&server, 0, sizeof(struct sockaddr_in));
    struct sockaddr *serverptr=(struct sockaddr *)&server;
	struct hostent* server_entity = NULL;




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
            /*!!!!!!!!!!!!!!!!!!!!!! check if it is a valid path in server*/
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

    //while(1){
        Client(socket_number, directory);
    //}


    exit(EXIT_SUCCESS);
}

void Client(int socket, char* directory){

    void* buffer[MAX_LENGTH] = {'\0'};
    int num_bytes_read = -1, files_remaining = -1;

    char* output_dir = (char*)calloc(strlen(OUT_DIR) + 6, sizeof(char)); //5 is for pid and 1 for '\0'

    snprintf(output_dir, strlen(OUT_DIR) + 6, "%s%d%c", OUT_DIR, getpid(), '\0');
    
    struct stat dir_info;
    memset(&dir_info, 0, sizeof(struct stat));
    if (stat(output_dir, &dir_info) < 0){
        if(mkdir(output_dir, 0744) == -1){
            perror("CLIENT: Could not create new directory");
            exit(EXIT_FAILURE);  
        }
    }
    


    size_t block_size = 0;

    printf("CLIENT SOCKET %d\n", socket);
    if (write(socket, directory, strlen(directory)) < 0)
        perror("CLIENT: Write directory name");
    
    if((num_bytes_read = read(socket, (char*)buffer, MAX_LENGTH)) < 0){
        perror("CLIENT: READ \"WRONG DIR NAME\" ");
    }
    printf("%s\n", (char*)buffer);
    if(!strcmp((char*)buffer, WRONG_MSG))
        exit(EXIT_FAILURE);
    else if(!strncmp((char*)buffer, FILES_SENT_MSG, strlen(FILES_SENT_MSG))){
        printf("before num bytes read: %d\n", num_bytes_read);
        num_bytes_read = num_bytes_read - strlen(FILES_SENT_MSG);  /* 1 is for "\n" */
        printf("after num bytes read: %d\n", num_bytes_read);
        char* num_files = (char*)calloc(num_bytes_read+1, sizeof(char));
        memcpy(num_files, (char*)buffer + strlen(FILES_SENT_MSG), num_bytes_read);
        files_remaining = atoi(num_files);
        printf("I will receive %d files from server\n", files_remaining);
        free(num_files);
    

        if((num_bytes_read = read(socket, (uint32_t*)buffer, sizeof(uint32_t))) < 0){
            perror("CLIENT: READ block size ");
        }
        block_size = ntohl(*(uint32_t*)buffer);
        printf("WITHOUT: block size %d\t WITH: block size %ld\n",  *(uint32_t*)buffer, block_size);
        
        if (write(socket, ACK_MSG, strlen(ACK_MSG)) < 0)
            perror("CLIENT: Write ACK message");
    }

    //int waiting = 1;
    do{
        memset(buffer, '\0', MAX_LENGTH*sizeof(char*));

        while((num_bytes_read = read(socket, (uint32_t*)buffer, sizeof(uint32_t))) < 0){
            perror("CLIENT: READ file length ");
        }
        int file_length = ntohl(*(uint32_t*)buffer);
        printf("WITHOUT: file length %d\t WITH: file length %d\n",  *(uint32_t*)buffer, file_length);

        while((num_bytes_read = read(socket, (uint32_t*)buffer, sizeof(uint32_t))) < 0){
            perror("CLIENT: READ file size ");
        }
        int file_size = ntohl(*(uint32_t*)buffer);

        printf("WITHOUT: file size %d\t WITH: file size %d\n\n",  *(uint32_t*)buffer, file_size);
        while((num_bytes_read = read(socket, (char*)buffer, file_length)) < 0){
            perror("CLIENT: READ file name ");
        }
        char* path = (char*)calloc(file_length+1, sizeof(char));
        memcpy(path, (char*)buffer, strlen((char*)buffer));
        printf("CLIENT GOT path: %s \n", path);

        int file_fd = -1;
        /* CREATE FILE -- DELETE IT IF EXISTS */
        if(strcmp(path, TERMINATION_MSG) != 0){
            
            file_fd = Resolve_FilePath(path, output_dir);
            if (write(socket, ACK_MSG, strlen(ACK_MSG)) < 0)
                perror("CLIENT: Write ACK message");


            printf("fd = %d wrote ack, before reading content\n", file_fd);
            /* GET THE CONTENT OF THE FILE*/


            int bytes_to_read = file_size;
            int bytes_read = 0;

            while(bytes_to_read){

                if(bytes_to_read < block_size)
                    block_size = bytes_to_read;
                //while(actual_bytes_read < block_size){
                    if((bytes_read = read(socket, buffer, block_size)) < 0){
                        perror("CLIENT: Read file content from socket");
                    }
                    //actual_bytes_read += bytes_read;
                //}
                if(write(file_fd, buffer, block_size) < 0){
                    perror("SERVER: WRITE file content");
                    exit(EXIT_FAILURE);
                }
                Clear_Buffer((char*)buffer, block_size);
                bytes_to_read -= block_size;
            }
            if(close(file_fd) == -1){
                perror("CLIENT: Close file");
                exit(EXIT_FAILURE);
            }

        }
           

        if (write(socket, ACK_MSG, strlen(ACK_MSG)) < 0)
            perror("CLIENT: Write ACK message");
        printf("wrote ack after copied the file\n");
        free(path);
    }while(strcmp((char*)buffer, TERMINATION_MSG) != 0);

    free(output_dir);

}
