#include "../headers/libraries.h"


int main(int argc, char* argv[]){


    if(argc != TOTAL_ARGS_CLIENT){
        Print_Error("Wrong number of arguments in the command line");
    }

    uint16_t port = -1;
    char* directory = NULL;
    //char directory[MAX_PATH_LENGTH] = {'\0'};
    DIR* dir_ptr = NULL;
    int socket_number = -1;
    struct in_addr server_address;
	struct hostent* server_entity = NULL;
    struct sockaddr_in server;
    struct sockaddr *serverptr = (struct sockaddr*)&server;

    memset(&server_address, 0, sizeof(struct in_addr));

    for(int i=1; i < TOTAL_ARGS_CLIENT; i+=2){

        if(!strcmp(argv[i], "-i")){
	        /* IPV dot-number into  binary form (network byte order) */
            if (inet_aton(argv[i+1], &server_address) == 0)
                Print_Error("IP address is not valid");
            if ((server_entity = gethostbyaddr((const char*)&server_address, sizeof(server_address), AF_INET)) == NULL){
                herror("Given IP-address could not be resolved\n");
                exit(EXIT_FAILURE);
            }
		    printf("IP-address:%s Resolved to: %s\n", argv[1],server_entity->h_name);
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
    	Print_Error("Error in creating socket");

    /* connect to server */

    server.sin_family = AF_INET;       /* Internet domain */
    memcpy(&server.sin_addr, server_entity->h_addr, server_entity->h_length);
    server.sin_port = port;         /* Server port */
    /* Initiate connection */
    if (connect(socket_number, serverptr, sizeof(server)) < 0)
	   Print_Error("Client could not connect to Server");
    
    printf("Connecting to %s in port %d\n", argv[1], port);


    exit(EXIT_SUCCESS);
}