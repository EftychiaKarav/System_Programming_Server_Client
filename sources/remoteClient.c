#include "../headers/Client.h"


int main(int argc, char* argv[]){

    if(argc != TOTAL_ARGS_CLIENT){
        Print_Error("Wrong number of arguments in the command line");
    }

    char* IP = NULL;
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
            if (inet_aton(argv[i+1], &server.sin_addr) == 0)
                Print_Error("IP address is not valid");
            if ((server_entity = gethostbyaddr((const char*)&server.sin_addr, sizeof(server.sin_addr), AF_INET)) == NULL){
                herror("Given IP-address could not be resolved\n");
                exit(EXIT_FAILURE);
            }
            IP = argv[i+1];
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
    server.sin_port = port;            /* Server port */
    
    /* Initiate connection */
    if (connect(socket_number, serverptr, sizeof(server)) < 0)
	   Print_Error("Client could not connect to Server");
    
    printf("Client's parameters are:\nServer IP: %s\nPort: %d\nDirectory: %s\n", IP, port, directory);
	printf("IP-address:%s --> Resolved to: %s\n", IP,server_entity->h_name);
    printf("Connecting to %s on port %d\n\n\n", IP, port);

    /* Client Process */
    Client(socket_number, directory);
    
    /* Close the socket */
    if(close(socket_number) == -1){
        Print_Error("CLIENT: Close socket");
    }

    /* Close file descriptors for stdin, stdout, stderr */
    close(STDOUT_FILENO);
    close(STDIN_FILENO);
    close(STDERR_FILENO);

    exit(EXIT_SUCCESS);
}