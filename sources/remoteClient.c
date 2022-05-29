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

    if(close(socket_number) == -1){
        perror("CLIENT: Close socket");
        exit(EXIT_FAILURE);
    }
    
    close(STDOUT_FILENO);
    close(STDIN_FILENO);
    close(STDERR_FILENO);

    exit(EXIT_SUCCESS);
}
/***************************************************************************************************************/




void Client(int socket, char* directory){

    char* buffer = (char*)calloc(MAX_LENGTH, sizeof(char));
    uint32_t *numbers_buffer = (uint32_t*)calloc(1, sizeof(uint32_t)); 
    int num_bytes_read = -1;

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
    int mess_length = strlen(CONFIRMATION_MSG) + strlen(directory) + strlen(DEFAULT_DIR);
    char* confirm_mess = (char*)calloc(mess_length + 1, sizeof(char));
    snprintf(confirm_mess, mess_length + 1, "%s%s%s%c", CONFIRMATION_MSG, DEFAULT_DIR, directory, '\0');
    printf("%s\n", (char*)buffer);
    if(!strncmp((char*)buffer, WRONG_MSG, strlen(WRONG_MSG)))
        exit(EXIT_FAILURE);
    else if(!strncmp((char*)buffer, confirm_mess, strlen(confirm_mess))){
        
        printf("Waiting to get the files from server\n");
        printf("buffer: %s\n", (char*)buffer);
        if (write(socket, ACK_MSG, strlen(ACK_MSG)) < 0)
            perror("CLIENT: Write ACK message");

        //Clear_Buffer((char*)buffer, strlen(confirm_mess));
        if((num_bytes_read = read(socket, numbers_buffer, sizeof(uint32_t))) < 0){
            perror("CLIENT: READ block size ");
        }

        block_size = ntohl(*numbers_buffer);
        printf("WITHOUT: block size %d\t WITH: block size %ld\n",  *(uint32_t*)buffer, block_size);
        
        if (write(socket, ACK_MSG, strlen(ACK_MSG)) < 0)
            perror("CLIENT: Write ACK message");
    }
    free(confirm_mess);
    //int waiting = 1;
    /*************************************************************************************************/
    printf("\n\nbefore start reading do -- while\n");

    char* content_buffer = (char*)calloc(block_size+1, sizeof(char));
    do{
        memset(numbers_buffer, 0, sizeof(uint32_t));
        Clear_Buffer(buffer, MAX_LENGTH);
        while((num_bytes_read = read(socket, numbers_buffer, sizeof(uint32_t))) < 0){
            perror("CLIENT: READ file length ");
        }
        int file_length = ntohl(*numbers_buffer);
        printf("WITHOUT: file length %d\t WITH: file length %d\n",  *numbers_buffer, file_length);

        while((num_bytes_read = read(socket, numbers_buffer, sizeof(uint32_t))) < 0){
            perror("CLIENT: READ file size ");
        }
        int file_size = ntohl(*numbers_buffer);
        
        printf("WITHOUT: file size %d\t WITH: file size %d\n",  *numbers_buffer, file_size);
        //memset((uint32_t*)buffer, 0, sizeof(uint32_t));
        while((num_bytes_read = read(socket, buffer, file_length)) < 0){
            perror("CLIENT: READ file name ");
            exit(EXIT_FAILURE);
        }
        printf("%s \n", buffer);

        char* path = (char*)calloc(file_length+1, sizeof(char));
        memcpy(path, (char*)buffer, strlen((char*)buffer));
        printf("CLIENT GOT path: %s \n", path);
        //Clear_Buffer((char*)buffer, MAX_LENGTH);

        int file_fd = -1;
        /* CREATE FILE -- DELETE IT IF EXISTS */
        if(strcmp(path, TERMINATION_MSG) != 0){
            
            file_fd = Resolve_FilePath(path, output_dir);
            if (write(socket, ACK_MSG, strlen(ACK_MSG)) < 0)
                perror("CLIENT: Write ACK message");


            printf("fd = %d wrote ack, before reading content\n", file_fd);
            /* GET THE CONTENT OF THE FILE*/

            Clear_Buffer((char*)buffer, MAX_LENGTH);

            int bytes_to_read = file_size;
            int bytes_read = 0;
            while(bytes_to_read){

                if(bytes_to_read < block_size)
                    block_size = bytes_to_read;
                //while(actual_bytes_read < block_size){
                    if((bytes_read = read(socket, content_buffer, block_size)) < 0){
                        perror("CLIENT: Read file content from socket");
                    }
                    //actual_bytes_read += bytes_read;
                //}
                if(write(file_fd, content_buffer, block_size) < 0){
                    perror("SERVER: WRITE file content");
                    exit(EXIT_FAILURE);
                }
                //memset((char*)buffer, '\0', MAX_LENGTH*sizeof(char*));
                Clear_Buffer(content_buffer, block_size);
                bytes_to_read -= block_size;
            }
            if(close(file_fd) == -1){
                perror("CLIENT: Close file");
                exit(EXIT_FAILURE);
            }

        }
        //Clear_Buffer((char*)buffer, MAX_LENGTH);
  
        //memset((char*)buffer, '\0', MAX_LENGTH*sizeof(char*));

        if (write(socket, ACK_MSG, strlen(ACK_MSG)) < 0)
            perror("CLIENT: Write ACK message");
        printf("wrote ack after copied the file\n\n");
        free(path);
    }while(strncmp((char*)buffer, TERMINATION_MSG, strlen(TERMINATION_MSG)) != 0);

    if (write(socket, TERM_MSG, strlen(TERM_MSG)) < 0)
        perror("CLIENT: Write ACK message");
    printf("wrote ack after copied the file\n");
    free(buffer);
    free(numbers_buffer);
    free(content_buffer);
    free(output_dir);

}
