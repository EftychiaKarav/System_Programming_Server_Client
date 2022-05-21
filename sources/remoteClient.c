#include "../headers/libraries.h"


int main(int argc, char* argv[]){


    if(argc != TOTAL_ARGS_CLIENT){
        Print_Error("Wrong number of arguments in the command line");
    }

    uint16_t port = -1;
    char* directory = NULL;
    //char directory[MAX_PATH_LENGTH] = {'\0'};
    DIR* dir_ptr = NULL;

    for(int i=1; i < TOTAL_ARGS_CLIENT; i+=2){

        if(!strcmp(argv[i], "-i")){

        }

        else if(!strcmp(argv[i], "-p")){
            if(atoi(argv[i+1]) <= 0)
                Print_Error("Port should be a positive number");
            port = atoi(argv[i+1]);
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





    exit(EXIT_SUCCESS);
}