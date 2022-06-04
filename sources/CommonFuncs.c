//KARAVANGELI EFTYCHIA - 1115201800062

#include "../headers/libraries.h"

/* prints error and exits */
void Print_Error(char* message){

    perror(message);
    exit(EXIT_FAILURE);

}


/* prints error along with a value and exits */
void Print_Error_Value(char* message, int error){

    printf("%s and error value: %d\n", message, error);
    exit(EXIT_FAILURE);
}


/* apply '\0' to a buffer */
void Clear_Buffer(char* buffer, int size){
    memset(buffer, '\0', size);
}


/* writes data to a file descriptor -- exits if there is an error */
void Send_Data(int file_desc, void* buffer, size_t total_bytes, char* error_msg){

    int bytes_to_send = total_bytes, bytes_written = 0;
    while(bytes_to_send){  /* do until all bytes have been sent */
        if((bytes_written = write(file_desc, buffer, bytes_to_send)) < 0){
            Print_Error(error_msg);
        }
        bytes_to_send -= bytes_written;  /* substract actual bytes written */
    }
    return;
}

/* reads data from a file descriptor -- exits if there is an error */
void Receive_Data(int file_desc, void* buffer, size_t total_bytes, char* error_msg){

    int bytes_to_receive = total_bytes, bytes_read = 0;
    while(bytes_to_receive){   /* do until all bytes have been sent */
        if((bytes_read = read(file_desc, buffer, bytes_to_receive)) < 0){
            Print_Error(error_msg);
        }
        bytes_to_receive -= bytes_read;   /* substract actual bytes read */
    }
    return;
}
