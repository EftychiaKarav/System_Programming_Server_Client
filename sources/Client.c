#include "../headers/Client.h"


/* Process of the remoteClient -> the client is connected to the dataServer through a socket
   and asks for a directory from the server in order to copy it in its file system*/
void Client(int socket, char* directory){

    char buffer[MAX_LENGTH+1] = {0};
    uint32_t* numbers_buffer = (uint32_t*)calloc(1, sizeof(uint32_t)); 
    
    /* 1. Send to the server the length of the directory name to acquire */
    uint16_t dir_length = htons(strlen(directory));          /* convert from host order to Network Byte Order */
    Send_Data(socket, &dir_length, sizeof(uint16_t), "CLIENT: Write directory length");

    /* 2. Send to the server the directory name to acquire */
    Send_Data(socket, directory, strlen(directory), "CLIENT: Write directory name");

    /* 3. Receive the length of the directory path in the server's filesystem */
    Receive_Data(socket, &dir_length, sizeof(uint16_t), "CLIENT: READ \"WRONG DIR LENGTH\" ");
    dir_length = ntohs(dir_length);        //convert from Network Byte Order to host order 

    /* 4. Receive the name of the directory path in the server's filesystem */
    Receive_Data(socket, buffer, dir_length, "CLIENT: READ \"WRONG DIR NAME\" ");

    /* 5. bulid a message to confirm what server has sent */
    int mess_length = strlen(CONFIRMATION_MSG) + strlen(directory) + strlen(DEFAULT_DIR);
    char* confirm_mess = (char*)calloc(mess_length + 1, sizeof(char));
    snprintf(confirm_mess, mess_length + 1, "%s%s%s%c", CONFIRMATION_MSG, DEFAULT_DIR, directory, '\0');
    
    size_t block_size = 0;
    if(!strncmp(buffer, WRONG_MSG, strlen(WRONG_MSG)))      /* server has sent "wrong dir name" */
        exit(EXIT_FAILURE);
    else if(!strncmp(buffer, confirm_mess, strlen(confirm_mess))){   /* server has sent "About to scan [dir name] " */
        
        /* 6. Send confirmation to the server */
        Send_Data(socket, ACK_MSG, strlen(ACK_MSG), "CLIENT: Write ACK message");
        
        /* 7. Receive block size from server */
        Receive_Data(socket, numbers_buffer, sizeof(uint32_t), "CLIENT: READ block size ");
        block_size = ntohl(*numbers_buffer);          //convert from Network Byte Order to host order 

        /* 8. Send confirmation to the server */
        Send_Data(socket, ACK_MSG, strlen(ACK_MSG), "CLIENT: Write ACK message");

    }
    free(confirm_mess);

    /* 9. client copies file in his file system */
    Client_CopyFiles(socket, buffer, block_size);

    /* 10. Send confirmation to the server */
    Send_Data(socket, TERM_MSG, strlen(TERM_MSG), "CLIENT: Write TERM message");

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

    /* 2. check if [output_dir] already exists */
    struct stat dir_info;
    memset(&dir_info, 0, sizeof(struct stat));
    if (stat(output_dir, &dir_info) < 0){ 
        if (errno == ENOENT)   /* if directory does not exist */
            if(mkdir(output_dir, 0744) == -1){
                if (errno != EEXIST)
                    Print_Error("CLIENT: Could not create output directory");
            }
    }

    char* content_buffer = (char*)calloc(block_size+1, sizeof(char));      //buffer for reading file content
    
    do{
        Clear_Buffer(buffer, MAX_LENGTH);
        
        /* 3. Receive the length of the file path name, file size and the file name */
        uint32_t file_size = 0;
        char* path = Client_Get_FileMetaData(socket, buffer, &file_size);

        int file_fd = -1;
        if(strncmp(path, TERMINATION_MSG, strlen(TERMINATION_MSG)) != 0){
            
            /* 4. Create server's file system hierarchy and create and open the new file to write to */
            file_fd = Client_Resolve_FilePath(path, output_dir);
            
            int bytes_to_recv = file_size;
            int bytes_read = 0;

            while(bytes_to_recv){

                if(bytes_to_recv < block_size)
                    block_size = bytes_to_recv;
                
                /* 5. Receive the content of the file block size by block size */
                Receive_Data(socket, content_buffer, block_size, "CLIENT: Read file content from socket");
                
                /* 5. Write the content of the file to the new file block size by block size */
                Send_Data(file_fd, (char*)content_buffer, block_size, "CLIENT: WRITE file content");
                
                bytes_to_recv -= block_size;
                Clear_Buffer(content_buffer, bytes_read);
            }
            /* 6. close file descriptor of the copied file */
            if(close(file_fd) == -1){      
                Print_Error("CLIENT: Close file after copying");
            }

        }
        printf("Received: %s\n\n", path);
        /* 7. Send confirmation to the server */
        Send_Data(socket, ACK_MSG, strlen(ACK_MSG), "CLIENT: Write ACK message");
        free(path);

    }while(strncmp(buffer, TERMINATION_MSG, strlen(TERMINATION_MSG)) != 0);

    free(content_buffer);
    free(output_dir);

}

/******************************************************************************************************************************/



/* Read 1. the length of the path of the file
        2. the size of the file 
        3. the name of the file and return the name of the name of the file*/
char* Client_Get_FileMetaData(int socket, char* buffer, uint32_t* file_size){
    
    uint16_t* length_buffer = (uint16_t*)calloc(1, sizeof(uint16_t));   /* buffer to read the filepath_length */
    memset(length_buffer, 0 , sizeof(uint16_t));

    uint32_t* size_buffer = (uint32_t*)calloc(1, sizeof(uint32_t));     /* buffers to read the file_size */ 
    memset(size_buffer, 0 , sizeof(uint32_t));

    /* 1. Receive the length of the path of the file */
    Receive_Data(socket, length_buffer, sizeof(uint16_t), "CLIENT: READ file path length ");
    uint16_t file_length = ntohs(*length_buffer);       //convert from Network Byte Order to host order 

    /* 2. Receive the size of the file */
    Receive_Data(socket, size_buffer, sizeof(uint32_t),"CLIENT: READ file size " );
    *file_size = ntohl(*size_buffer);                   //convert from Network Byte Order to host order

    /* 3. Receive the name of the path of the file along with the name of the file */
    Receive_Data(socket, buffer, file_length,"CLIENT: READ file name " );

    /* 4. copy file path to a new variable */
    char* path = (char*)calloc(file_length+1, sizeof(char));
    memcpy(path, buffer, strlen(buffer));

    free(size_buffer);
    free(length_buffer);

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

    /* 1. start building the file system hierarchy of the server at the client */
    /* The directory sent from the server will be copied inside [output_dir] */

    /* [copied_path] --> the substring of the whole path string currently copied into the [output_dir] */
    char* copied_path = (char*)calloc(strlen(path)+strlen(output_dir)+ 1, sizeof(char));
    memcpy(copied_path, output_dir, strlen(output_dir));
    
    
    struct stat node_info;      /* struct to check if a directory exists, while reading the path string*/
    short int isDirectory = 1;  /* given a path string all names are directories except for the last one; it is a file */
    int new_file_fd = -1;       /* file desc for the new file which will be created */

    
    while(strlen(start) != 0){  /* scan the whole path string until we reach the end of it */

        end = strstr(start, "/");  // 2. search for '/' --> seperates the directories and files
        
        /* '/' not found --> there is no other directory; the last substring is a file */
        if(end == NULL){
            isDirectory = 0;
            end = start + strlen(start) - 1;   // points to the last character of the string
        }
        memset(&node_info, 0, sizeof(struct stat));

        /* 3. copy the substring until the '/' to the [copied_path] */
        memcpy(copied_path + strlen(copied_path), start, strlen(start) - strlen(end) +1);
        
        int node_status = stat(copied_path, &node_info);
        
        /* 4. create directory if it doesn't exist */
        if(isDirectory){
            if (node_status < 0){
                if(errno == ENOENT)   /* if dir does not exist */
                    if(mkdir(copied_path, 0744) == -1){
                        if (errno != EEXIST)
                            Print_Error("CLIENT: Could not create new directory");
                    }
            }
        }
        else{          /* 4. CREATE FILE -- DELETE IT IF EXISTS */
            if (node_status == 0){
                if (unlink(copied_path) < 0){
                    Print_Error("CLIENT: Could not delete already existing file ");
                }
            }
            if ( (new_file_fd = open(copied_path, O_CREAT|O_RDWR, 0644)) == -1){
                Print_Error("CLIENT: Creating new file");
            }
        }       
        start = end + 1;   // 5. [start] points to character after the '/'
    }
    
    free(copied_path);
    return new_file_fd;    /* file descriptor of the new file */

}


/******************************************************************************************************************************/