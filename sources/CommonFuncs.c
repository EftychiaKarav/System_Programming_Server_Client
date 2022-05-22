#include "../headers/libraries.h"


void Print_Error(char* message){

    printf("%s\n", message);
    exit(EXIT_FAILURE);

}

void Print_Error_Value(char* message, int error){

    printf("%s and error value: %d\n", message, error);
    exit(EXIT_FAILURE);
}