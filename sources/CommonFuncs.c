#include "../headers/libraries.h"


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

void Send_Data(int file_desc, void* buffer, size_t total_bytes, char* error_msg){

    int bytes_to_send = total_bytes, bytes_written = 0;
    while(bytes_to_send){
        if((bytes_written = write(file_desc, buffer, bytes_to_send)) < 0){
            perror(error_msg);
            exit(EXIT_FAILURE);
        }
        bytes_to_send -= bytes_written;
    }
    return;
}

void Receive_Data(int file_desc, void* buffer, size_t total_bytes, char* error_msg){

    int bytes_to_receive = total_bytes, bytes_read = 0;
    while(bytes_to_receive){
        //printf("%s\n", (char*)buffer);
        if((bytes_read = read(file_desc, buffer, bytes_to_receive)) < 0){
            perror(error_msg);
            exit(EXIT_FAILURE);
        }
        bytes_to_receive -= bytes_read;
    }
    return;
}
