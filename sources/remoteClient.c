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