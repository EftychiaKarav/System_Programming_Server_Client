//KARAVANGELI EFTYCHIA - 1115201800062

#ifndef LIBRARIES_H_
#define LIBRARIES_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <sys/socket.h>	    
#include <netinet/in.h>	     
#include <netdb.h>	         
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_LENGTH 4096
#define DEFAULT_DIR "/home/users/"                     /* all path directories requested by client are relative to this path */
//#define DEFAULT_DIR "/mnt/d/DOCUMENTS/"              /* for my laptop */ 
//#define DEFAULT_DIR "/mnt/d/Docs/"                   /* for my laptop */
#define ACK_MSG "ACK"                                  /* client sends it to server to notify him that he has received data */
#define CONFIRMATION_MSG "About to scan directory: "   /* server sends to client if he has received correct dir name */
#define WRONG_MSG "WRONG DIRECTORY NAME"               /* server sends to client if he has received wrong dir name */
#define TERMINATION_MSG "ALL FILES WERE TRANSFERED"    /* server sends to client to notify him to terminate */
#define TERM_MSG "END"                                 /* client sends it to server and the respective comm thread exits */


/* prints error and exits */
void Print_Error(char*);

/* prints error along with a value and exits */
void Print_Error_Value(char*, int);

/* apply '\0' to a buffer */
void Clear_Buffer(char*, int);

/* writes data to a file descriptor -- exits if there is an error */
void Send_Data(int, void*, size_t, char*);

/* reads data from a file descriptor -- exits if there is an error */
void Receive_Data(int, void*, size_t, char*);

#endif