#include "../headers/Server.h"

int main(int argc, char* argv[]){

    if(argc != TOTAL_ARGS_SERVER){
        Print_Error("Wrong number of arguments in the command line");
    }

    /* Check if the command line arguments are correct */

    int thread_pool_size = -1, queue_size = -1, socket_number = -1;
    uint16_t port = -1;
    size_t block_size = -1;

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


    printf("\nServer's parameters are:\nPort: %d\nThread_Pool_Size: %d\nQueue_Size: %d\nBlock_Size: %ld\n",
          port, thread_pool_size, queue_size, block_size);
    printf("Server was successfully initialized...\n");

    /* Sigaction for server */
    struct sigaction server_action;
    memset(&server_action, 0, sizeof(struct sigaction));
    server_action.sa_handler = Stop_Server;   //handle ^C 
	sigaction(SIGINT, &server_action, NULL);
    server_action.sa_handler = SIG_DFL;   //handle ^Z the default way
    sigaction(SIGTSTP, &server_action, NULL);


    struct sockaddr_in server, client;
    socklen_t clientlen = 1;   /* just a value to initialize it --> is changed after a client connects to the server */
    struct sockaddr *serverptr = (struct sockaddr *)&server;   /* socket address for server */
    struct sockaddr *clientptr = (struct sockaddr *)&client;   /* socket address for client */
    struct hostent* client_entity = NULL;

    memset(&server, 0, sizeof(struct sockaddr_in));
    memset(&client, 0, sizeof(struct sockaddr_in));


    /* Create socket */
    if ((socket_number = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        Print_Error("Server: Could not create socket");
    
    server.sin_family = AF_INET;                  /* Internet domain */
    server.sin_addr.s_addr = htonl(INADDR_ANY);   /* Connect to any address */
    server.sin_port = port;                       /* given port */
    
    int value = 1;
    
    /* Use setsockopt in order to use the socket as fast as possible when server closes and runs again */
    if (setsockopt(socket_number, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(int)) < 0)
        Print_Error("Error in sockopt");
    printf("SERVER's IP is: %d, or %s\n", server.sin_addr.s_addr, inet_ntoa(server.sin_addr));
    
    /* Bind socket to address */
    if (bind(socket_number, serverptr, sizeof(server)) < 0){
        Print_Error("Server: Could not bind");
    }
    
    /* Listen for connections --> max 1000 Clients */
    if (listen(socket_number, 1000) < 0)
        Print_Error("Server: listen failed");
    
    printf("Listening for connections to port %d\n", port);

    /* initialize queue for saving file names with the respective socket to which the file should be sent */
    if(!Queue_Exists(Files_Queue)){
        Files_Queue = Queue_Initialize();
    }

    /* arguments to pass to the worker threads' (aka ThreadPool) start routine */
    Worker_Threads_Args args;
    memset(&args, 0, sizeof(Worker_Threads_Args));
    args.block_size = block_size;
    args.queue_size = queue_size;
    args.total_worker_threads = thread_pool_size;

    /* server creates [thread_pool_size] worker threads */
    ThreadPool_Initialize(&args);
    int new_socket_number = -1;


    /* RUNS constantly until ^C is typed */
    while(RUNNING){

        /* accepts connection from a client */
    	if ((new_socket_number = accept(socket_number, clientptr, &clientlen)) < 0){
            if(!RUNNING) break;
            Print_Error("Server could not accept connection");
        }
    	/* Find client's address */

    	if ((client_entity = gethostbyaddr((char *) &client.sin_addr, sizeof(client.sin_addr), client.sin_family)) == NULL){
            herror("Server could not resolve client's IP address");
            exit(EXIT_FAILURE);
        }
        printf("Accepted connection from %s who is listening on port %d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
        printf("Accepted connection from %s\n", client_entity->h_name);

        /* queue to keep a socket and a mutex */
        if(!Queue_Exists(Mutex_Socket_Queue)){
            Mutex_Socket_Queue = Queue_Initialize();
        }

        pthread_t communication_thread;    /* new coommunication thread for a new client */
        /* arguments to pass to the communaction thread's start routine */
        Commun_Threads_Args* args;
        args = (Commun_Threads_Args*)calloc(1, sizeof(Commun_Threads_Args));
        args->socket = new_socket_number;
        args->block_size = block_size;
        args->queue_size = queue_size;
        
        int err = 0;
        if ((err = pthread_create(&communication_thread, NULL, Server, (void*)(args))) != 0) { /* New thread */
            Print_Error_Value("Error in pthread_create", err);
        }
        // printf("ORIGINAL THREAD: [%ld] COMMUNICATION THREAD: [%ld]\n", 
        //     pthread_self(), communication_thread);

    }

    /* Stop listening for connections */
    if(close(socket_number) == -1){
        Print_Error("SERVER: Close socket");
    }

    /* some clean up */
    ThreadPool_Destroy(&args);
    Queue_Destroy(Files_Queue);
    Queue_Destroy(Mutex_Socket_Queue);

    /* Close file descriptors for stdin, stdout, stderr */
    close(STDOUT_FILENO);
    close(STDIN_FILENO);
    close(STDERR_FILENO);

    exit(EXIT_SUCCESS);
}
/********************************************************************************************************************/
