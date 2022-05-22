#include "../headers/libraries.h"





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
    setsockopt(socket_number, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(int));
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



void* Server_Job(void* arg){

    int socket = *(int*)arg;
    printf("NEW THREAD:  %ld\n", pthread_self());
    int err;
    if ((err = pthread_detach(pthread_self())) != 0) {/* Detach thread */
       Print_Error_Value("Error in pthread_detach", err);
    }

    sleep(10);
    printf("THREAD %ld is exiting\n", pthread_self());
    pthread_exit(NULL);

}