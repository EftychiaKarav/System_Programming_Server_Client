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
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/select.h>
#include <errno.h>
#include <features.h>
#include <dirent.h>
#include <stdint.h>
#include <sys/socket.h>	     /* sockets */
#include <netinet/in.h>	     /* internet sockets */
#include <netdb.h>	         /* gethostbyaddr */
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_LENGTH 4096
#define DEFAULT_DIR "/home/users/sdi1800062/Documents/"
//#define DEFAULT_DIR "/mnt/d/DOCUMENTS/"
#define WRONG_MSG "WRONG DIRECTORY NAME"
#define ACK_MSG "ACK"
#define CONFIRMATION_MSG "About to scan directory: "
#define TERMINATION_MSG "ALL FILES WERE TRANSFERED"
#define TERM_MSG "END"

extern int RUNNING;


void Print_Error(char*);
void Print_Error_Value(char*, int);
void Clear_Buffer(char*, int);

#endif