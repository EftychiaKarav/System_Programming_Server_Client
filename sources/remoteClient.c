#include "../headers/libraries.h"


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

    char buffer[MAX_LENGTH] = {'\0'};
    int num_bytes_read = -1, files_remaining = -1;
    if (write(socket, directory, strlen(directory)) < 0)
        perror("CLIENT: Write directory name");
    
    if((num_bytes_read = read(socket, buffer, MAX_LENGTH)) < 0){
        perror("CLIENT: READ \"WRONG DIR NAME\" ");
    }
    if(!strcmp(buffer, WRONG_MSG))
        exit(EXIT_FAILURE);
    else if(!strncmp(buffer, FILES_SENT_MSG, strlen(FILES_SENT_MSG))){
        num_bytes_read = num_bytes_read - strlen(FILES_SENT_MSG) - 1;  /* 1 is for "\n" */
        char* num_files = (char*)calloc(num_bytes_read, sizeof(char));
        memcpy(num_files, buffer + strlen(FILES_SENT_MSG), num_bytes_read);
        files_remaining = atoi(num_files);
        printf("I will receive %d files from server\n", files_remaining);
    }
    while(files_remaining){



    }


}
