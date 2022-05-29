#include "../headers/libraries.h"
int RUNNING = 1;


void Print_Error(char* message){

    printf("%s\n", message);
    exit(EXIT_FAILURE);

}

void Print_Error_Value(char* message, int error){

    printf("%s and error value: %d\n", message, error);
    exit(EXIT_FAILURE);
}

void Clear_Buffer(char* buffer, int size){
    
    memset(buffer, '\0', size);
}


void Stop_Server(int sugnum){
    printf("got signal");
    RUNNING = 0;
}